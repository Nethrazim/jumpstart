#include <atomic>
#include <thread>
#include <chrono>

#include "app-router.h"
#include "http_response.h"
#include "blocking_queue.h"
#include "request_handler.h"
#include "tcp_ip_connection.h"

extern AppRouter g_router;
extern BlockingQueue<HttpResponse> g_responseQueue;
extern std::atomic<bool> g_running;

bool parseHttpRequest(std::string& buffer, HttpRequest* request);

void RequestHandler::pushHttpRequest(HttpRequest* req)
{
	requestQueue_.emplace(req);
}

void RequestHandler::closeConnection(socket_t fd) {
    auto it = tcpIpConnections_.find(fd);
    if (it != tcpIpConnections_.end()) {
        CLOSE_SOCKET(fd);
        tcpIpConnections_.erase(it);
    }
}

void RequestHandler::handleRead(socket_t fd) {
    char buf[4096];

    while (true) {
        int n = recv(fd, buf, sizeof(buf), 0);

        if (n > 0) {
            auto it = tcpIpConnections_.find(fd);
            if (it == tcpIpConnections_.end())
                return;
            it->second.readBuf.append(buf, n);
        }
        else if (n == 0) {
            std::cout << "Client closed " << fd << "\n";
            //TODO remove connection if client conn closed
            closeConnection(fd);
            return;
        }
        else {
            int err = GET_SOCKET_ERROR();
            if (err == WOULDBLOCK_ERROR)
                break;
            std::cerr << "recv error " << err << " on " << fd << "\n";
            closeConnection(fd);
            return;
        }
    }

    HttpRequest* req = new HttpRequest();
    {
        auto it = tcpIpConnections_.find(fd);
        if (it == tcpIpConnections_.end()) {
            delete req;
            return;
        }

        if (!parseHttpRequest(it->second.readBuf, req)) {
            delete req;
    
        }
    }

    req->fd = fd;

    requestQueue_.push(req);
}


void RequestHandler::handleWrite(socket_t fd) {


    auto it = tcpIpConnections_.find(fd);

    if (it == tcpIpConnections_.end())
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
        int err = GET_SOCKET_ERROR();
        if (err != WOULDBLOCK_ERROR) {
            std::cerr << "send error " << err << " on " << fd << "\n";
            CLOSE_SOCKET(fd);
            tcpIpConnections_.erase(it);
        }
    }
}


void RequestHandler::drainWorkerResponses() {
    HttpResponse resp;

    while (!responseQueue_.empty()) {
        
        resp = responseQueue_.front();
        responseQueue_.pop();

        if (resp.fd == INVALID_SOCKET)
            continue;

        auto it = tcpIpConnections_.find(resp.fd);
        if (it == tcpIpConnections_.end())
            continue;

        it->second.writeBuf += resp.raw;
        it->second.wantWrite = true;
    }
}

//TODO : i need to check if socket is ready for write, if not retry 
void RequestHandler::loop()
{
    while (g_running) {

        // If no connections, sleep to avoid busy-waiting
        if (tcpIpConnections_.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        std::vector<pollfd_t> fileDescriptors;

        for (auto& kv : tcpIpConnections_)
        {
            pollfd_t pFd{};
            pFd.fd = kv.first;
            pFd.events = POLLRDNORM;
            if (kv.second.wantWrite) {
                pFd.events |= POLLWRNORM;
            }

            fileDescriptors.push_back(pFd);
        }

        int n = POLL(fileDescriptors.data(), fileDescriptors.size(), 100);

        if (n == SOCKET_ERROR) {
            int err = GET_SOCKET_ERROR();
            std::cerr << "Poll error: " << err << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // If poll returned 0 (timeout, no activity), continue
        if (n == 0) {
            continue;
        }

        for (auto& p : fileDescriptors) {

            if (p.revents & POLLRDNORM) {
                handleRead(p.fd);
            }

            if (p.revents & POLLWRNORM)
            {
                handleWrite(p.fd);
            }

            if (p.revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                closeConnection(p.fd);
            }
        }

        // Process one request from queue
        HttpRequest* req = nullptr;

        if (!requestQueue_.empty()) {
            req = requestQueue_.front();
            requestQueue_.pop();
        } else {
            // No requests to process, brief sleep before next poll
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        if (!g_running || req->fd == INVALID_SOCKET) {
            delete req;
            break;
        }

        HttpResponse resp;
        resp.fd = req->fd;

        g_router.dispatch(*req, resp);

        if (resp.raw.empty())
        {
            delete req;
            continue;
        }


        int sendN = send(resp.fd, resp.raw.data(), (int)resp.raw.size(), 0);

        if (sendN > 0) {
            resp.raw.erase(0, sendN);
        }
        else
        {
            int err = GET_SOCKET_ERROR();
            if (err != WOULDBLOCK_ERROR) {
                std::cerr << "send error " << err << " on " << resp.fd << "\n";
                CLOSE_SOCKET(resp.fd);

            }
        }

        delete req; 
    }

    drainWorkerResponses();
}