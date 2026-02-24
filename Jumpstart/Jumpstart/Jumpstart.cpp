// tiny_http_server.cpp
// Build (MSVC example):
//   cl /EHsc tiny_http_server.cpp ws2_32.lib

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#include <windows.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

#pragma comment(lib, "ws2_32.lib")

// --------------------- Simple thread-safe queue ---------------------

template<typename T>
class BlockingQueue {
public:
    void push(T v) {
        {
            std::lock_guard<std::mutex> lock(m_);
            q_.push(std::move(v));
        }
        cv_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(m_);
        cv_.wait(lock, [&] { return !q_.empty() || stop_; });
        if (stop_ && q_.empty())
            return T{};
        T v = std::move(q_.front());
        q_.pop();
        return v;
    }

    bool try_pop(T& out) {
        std::lock_guard<std::mutex> lock(m_);
        if (q_.empty()) return false;
        out = std::move(q_.front());
        q_.pop();
        return true;
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(m_);
            stop_ = true;
        }
        cv_.notify_all();
    }

private:
    std::mutex m_;
    std::condition_variable cv_;
    std::queue<T> q_;
    bool stop_ = false;
};

// --------------------- HTTP types ---------------------

struct HttpRequest {
    SOCKET fd = INVALID_SOCKET;
    std::string method;
    std::string path;
    std::string body;
};

struct HttpResponse {
    SOCKET fd = INVALID_SOCKET;
    std::string raw;
};

struct Connection {
    SOCKET fd = INVALID_SOCKET;
    std::string readBuf;
    std::string writeBuf;
    bool wantWrite = false;
};

// --------------------- Globals (for demo) ---------------------

static std::unordered_map<SOCKET, Connection> g_connections;
static std::mutex g_connMutex;

static BlockingQueue<HttpRequest> g_reqQueue;
static BlockingQueue<HttpResponse> g_respQueue;

static std::atomic<bool> g_running{ true };

// --------------------- Very tiny HTTP parser ---------------------
// Extremely naive: looks for "\r\n\r\n" and treats everything before as headers.
// Ignores Content-Length and body for simplicity.

bool tryParseHttpRequest(std::string& buffer, HttpRequest& out) {
    std::size_t pos = buffer.find("\r\n\r\n");
    if (pos == std::string::npos)
        return false;

    std::string headerPart = buffer.substr(0, pos);
    buffer.erase(0, pos + 4);

    // First line: METHOD PATH HTTP/1.1
    std::size_t lineEnd = headerPart.find("\r\n");
    if (lineEnd == std::string::npos)
        return false;

    std::string requestLine = headerPart.substr(0, lineEnd);
    std::size_t mEnd = requestLine.find(' ');
    if (mEnd == std::string::npos) return false;
    std::size_t pEnd = requestLine.find(' ', mEnd + 1);
    if (pEnd == std::string::npos) return false;

    out.method = requestLine.substr(0, mEnd);
    out.path = requestLine.substr(mEnd + 1, pEnd - (mEnd + 1));
    out.body.clear(); // ignoring body

    return true;
}

// --------------------- Worker thread ---------------------

void workerThreadFunc(int id) {
    while (g_running) {
        HttpRequest req = g_reqQueue.pop();
        if (!g_running || req.fd == INVALID_SOCKET)
            break;

        HttpResponse resp;
        resp.fd = req.fd;

        std::string body = "Hello from worker " + std::to_string(id) +
            " path=" + req.path + "\n";
       body = u8R"(<!DOCTYPE html>
            <body>
            <span style="font-size:24px; color: red;">Mâța pe gheață</span>
            </body>
            </html>)";

        resp.raw = "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Connection: keep-alive\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "\r\n" +
            body;

        g_respQueue.push(std::move(resp));
    }
}

// --------------------- Connection helpers ---------------------

void closeConnection(SOCKET fd) {
    std::lock_guard<std::mutex> lock(g_connMutex);
    auto it = g_connections.find(fd);
    if (it != g_connections.end()) {
        closesocket(fd);
        g_connections.erase(it);
    }
}

void handleAccept(SOCKET listenSock) {
    while (true) {
        SOCKET client = accept(listenSock, nullptr, nullptr);
        if (client == INVALID_SOCKET) {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK)
                break;
            std::cerr << "accept error: " << err << "\n";
            break;
        }

        u_long mode = 1;
        ioctlsocket(client, FIONBIO, &mode);

        std::lock_guard<std::mutex> lock(g_connMutex);
        g_connections.emplace(client, Connection{ client });
        std::cout << "Accepted client " << client << "\n";
    }
}

void handleRead(SOCKET fd) {
    char buf[4096];

    while (true) {
        int n = recv(fd, buf, sizeof(buf), 0);
        if (n > 0) {
            std::lock_guard<std::mutex> lock(g_connMutex);
            auto it = g_connections.find(fd);
            if (it == g_connections.end())
                return;
            it->second.readBuf.append(buf, n);
        }
        else if (n == 0) {
            std::cout << "Client closed " << fd << "\n";
            closeConnection(fd);
            return;
        }
        else {
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK)
                break;
            std::cerr << "recv error " << err << " on " << fd << "\n";
            closeConnection(fd);
            return;
        }
    }

    while (true) {
        HttpRequest req;
        {
            std::lock_guard<std::mutex> lock(g_connMutex);
            auto it = g_connections.find(fd);
            if (it == g_connections.end())
                return;
            if (!tryParseHttpRequest(it->second.readBuf, req))
                break;
        }
        req.fd = fd;
        g_reqQueue.push(std::move(req));
    }
}

void handleWrite(SOCKET fd) {
    std::lock_guard<std::mutex> lock(g_connMutex);
    auto it = g_connections.find(fd);
    if (it == g_connections.end())
        return;

    Connection& conn = it->second;
    if (conn.writeBuf.empty()) {
        conn.wantWrite = false;
        return;
    }

    int n = send(fd, conn.writeBuf.data(), (int)conn.writeBuf.size(), 0);
    if (n > 0) {
        conn.writeBuf.erase(0, n);
        if (conn.writeBuf.empty())
            conn.wantWrite = false;
    }
    else {
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) {
            std::cerr << "send error " << err << " on " << fd << "\n";
            closesocket(fd);
            g_connections.erase(it);
        }
    }
}

void drainWorkerResponses() {
    HttpResponse resp;
    while (g_respQueue.try_pop(resp)) {
        if (resp.fd == INVALID_SOCKET)
            continue;
        std::lock_guard<std::mutex> lock(g_connMutex);
        auto it = g_connections.find(resp.fd);
        if (it == g_connections.end())
            continue;
        it->second.writeBuf += resp.raw;
        it->second.wantWrite = true;
    }
}

// --------------------- I/O loop ---------------------

void ioLoop(SOCKET listenSock) {
    while (g_running) {
        std::vector<WSAPOLLFD> fds;

        WSAPOLLFD lf{};
        lf.fd = listenSock;
        lf.events = POLLRDNORM;
        fds.push_back(lf);

        {
            std::lock_guard<std::mutex> lock(g_connMutex);
            fds.reserve(1 + g_connections.size());
            for (auto& kv : g_connections) {
                WSAPOLLFD p{};
                p.fd = kv.first;
                p.events = POLLRDNORM;
                if (kv.second.wantWrite)
                    p.events |= POLLWRNORM;
                fds.push_back(p);
            }
        }

        int n = WSAPoll(fds.data(), (ULONG)fds.size(), 100);
        if (n == SOCKET_ERROR) {
            int err = WSAGetLastError();
            std::cerr << "WSAPoll error: " << err << "\n";
            continue;
        }

        for (auto& p : fds) {
            if (p.fd == listenSock) {
                if (p.revents & POLLRDNORM)
                    handleAccept(listenSock);
            }
            else {
                if (p.revents & POLLRDNORM)
                    handleRead(p.fd);
                if (p.revents & POLLWRNORM)
                    handleWrite(p.fd);
                if (p.revents & (POLLERR | POLLHUP | POLLNVAL))
                    closeConnection(p.fd);
            }
        }

        drainWorkerResponses();
    }
}

// --------------------- Main ---------------------

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) {
        std::cerr << "socket failed\n";
        return 1;
    }

    u_long mode = 1;
    ioctlsocket(listenSock, FIONBIO, &mode);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8080);

    if (bind(listenSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "bind failed\n";
        return 1;
    }

    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed\n";
        return 1;
    }

    std::cout << "Listening on port 8080\n";

    // Start worker threads
    const int workerCount = 4;
    std::vector<std::thread> workers;
    for (int i = 0; i < workerCount; ++i)
        workers.emplace_back(workerThreadFunc, i);

    // I/O loop in main thread (or you could move it to a separate thread)
    ioLoop(listenSock);

    // Shutdown
    g_running = false;
    g_reqQueue.stop();
    g_respQueue.stop();

    for (auto& t : workers)
        t.join();

    closesocket(listenSock);
    WSACleanup();
    return 0;
}
