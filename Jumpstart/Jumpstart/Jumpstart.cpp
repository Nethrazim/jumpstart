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

#include "http_request.h"
#include "http_response.h"
#include "tcp_ip_connection.h"
#include "blocking_queue.h"
#include "app-router.h"
#include "tcp_ip_listener.h"
#include "request_handler.h"

#pragma comment(lib, "ws2_32.lib")

using std::cerr; 
// --------------------- HTTP types ---------------------


extern AppRouter g_router;
extern std::unordered_map<SOCKET, TcpIpConnection> g_tcpIpConnections;
extern BlockingQueue<HttpRequest> g_requestQueue;
extern BlockingQueue<HttpResponse> g_responseQueue;
extern std::vector<std::thread> g_workers;
extern std::vector<RequestHandler*> g_requestHandlers;

static std::mutex g_connMutex;

void placeHttpRequest(HttpRequest* req);
static std::atomic<bool> g_running{ true };

bool tryParseHttpRequest(std::string& buffer, HttpRequest* out) {
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

    out->method = requestLine.substr(0, mEnd);
    out->path = requestLine.substr(mEnd + 1, pEnd - (mEnd + 1));
    out->body.clear(); // ignoring body

    return true;
}

// --------------------- Connection helpers ---------------------

void closeConnection(SOCKET fd) {
    std::lock_guard<std::mutex> lock(g_connMutex);
    auto it = g_tcpIpConnections.find(fd);
    if (it != g_tcpIpConnections.end()) {
        closesocket(fd);
        g_tcpIpConnections.erase(it);
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

        g_tcpIpConnections.emplace(client, TcpIpConnection{ client });
        
        std::cout << "Accepted client " << client << "\n";
    }
}

void handleRead(SOCKET fd) {
    char buf[4096];

    while (true) {
        int n = recv(fd, buf, sizeof(buf), 0);
        if (n > 0) {
            std::lock_guard<std::mutex> lock(g_connMutex);
            auto it = g_tcpIpConnections.find(fd);
            if (it == g_tcpIpConnections.end())
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
        HttpRequest* req = new HttpRequest();
        {
            std::lock_guard<std::mutex> lock(g_connMutex);
            auto it = g_tcpIpConnections.find(fd);
            if (it == g_tcpIpConnections.end())
                return;
            if (!tryParseHttpRequest(it->second.readBuf, req))
                break;
        }
        req->fd = fd;

        placeHttpRequest(req);
    }
}   

void placeHttpRequest(HttpRequest* req)
{
    static int placeId = 0;
    g_requestHandlers.at(placeId)->pushHttpRequest(req);
    
    placeId++;
    if (placeId > g_requestHandlers.size() - 1)
    {
        placeId = 0;
    }
}

void handleWrite(SOCKET fd) {

    std::lock_guard<std::mutex> lock(g_connMutex);

    auto it = g_tcpIpConnections.find(fd);

    if (it == g_tcpIpConnections.end())
        return;

    TcpIpConnection& conn = it->second;

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
            g_tcpIpConnections.erase(it);
        }
    }
}

void drainWorkerResponses() {
    HttpResponse resp;
    while (g_responseQueue.try_pop(resp)) {
        if (resp.fd == INVALID_SOCKET)
            continue;
        std::lock_guard<std::mutex> lock(g_connMutex);
        auto it = g_tcpIpConnections.find(resp.fd);
        if (it == g_tcpIpConnections.end())
            continue;
        it->second.writeBuf += resp.raw;
        it->second.wantWrite = true;
    }
}

void tcpServerLoop(SOCKET listenSock) {

	while (g_running) {

		std::vector<WSAPOLLFD> fileDescriptors;

		WSAPOLLFD lf{};
		lf.fd = listenSock;
		lf.events = POLLRDNORM;
		fileDescriptors.push_back(lf);

		for (auto& kv : g_tcpIpConnections) {
			WSAPOLLFD pFd{};
			pFd.fd = kv.first;
			pFd.events = POLLRDNORM;
			if (kv.second.wantWrite) {
				pFd.events |= POLLWRNORM;
			}
			fileDescriptors.push_back(pFd);
		}

		int n = WSAPoll(fileDescriptors.data(), (ULONG)fileDescriptors.size(), 100);

		if (n == SOCKET_ERROR) {
			int err = WSAGetLastError();
			std::cerr << "WSAPoll error: " << err << "\n";
			continue;
		}

		for (auto& p : fileDescriptors) {
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



Response* handleGet(const Request& req) {
    Response* resp = new Response();

    std::string body = "Hello from GET " + req.path + "\n";
    body += u8R"(<!DOCTYPE html>
        <body>
        <span style="font-size:24px; color: red;">Mâța pe gheață</span>
        </body>
        </html>)";
    resp->raw = "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" +
        body;

    return resp;
}


Response* handleSubGet(const Request& req) {
    Response* resp = new Response();

    std::string body = "BYEBYE from GET " + req.path + "\n";
    body += u8R"(<!DOCTYPE html>
        <body>
        <span style="font-size:24px; color: red;">Mâța pe gheață</span>
        </body>
        </html>)";
    resp->raw = "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "\r\n" +
        body;

    return resp;
}

extern TcpIpListener* g_tcpIpListener;

void asc(HttpRequest&& r)
{
    HttpRequest r2(std::move(r));
}

int main() {
    g_router.setRoutes();

    g_tcpIpListener = TcpIpListener::getInstance();
    g_tcpIpListener->init();

    if (!g_tcpIpListener->isListening)
    {
        cerr << "TcpIpListener failed!\n";
    }

    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    DWORD nrOfProcessors = sysinfo.dwNumberOfProcessors;

    // Start worker threads
    std::vector<std::thread> workers;

    for (int i = 0; i < nrOfProcessors; ++i)
    {
        RequestHandler* requestHandler = new RequestHandler();
        workers.emplace_back(&RequestHandler::run, requestHandler);
        g_requestHandlers.push_back(requestHandler);
    }
        

    // I/O loop in main thread (or you could move it to a separate thread)
    tcpServerLoop(g_tcpIpListener->getListenSocket());

    // Shutdown
    g_running = false;
    g_requestQueue.stop();
    g_responseQueue.stop();

    for (auto& t : workers)
        t.join();

    g_tcpIpListener->release();
    return 0;
}
