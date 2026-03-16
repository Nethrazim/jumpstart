#include "platform.h"
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

using std::cerr; 

extern TcpIpListener* g_tcpIpListener;
extern std::unordered_map<socket_t, TcpIpConnection> g_tcpIpConnections;

extern AppRouter g_router;
extern BlockingQueue<HttpRequest> g_requestQueue;
extern BlockingQueue<HttpResponse> g_responseQueue;

extern std::vector<std::thread> g_workers;
extern std::vector<RequestHandler*> g_requestHandlers;


static std::mutex g_connMutex;

void placeHttpRequest(HttpRequest* req);
extern std::atomic<bool> g_running;  // Defined in global_variables.cpp
bool parseHttpRequest(std::string&, HttpRequest* out);


bool tryParseHttpRequest(std::string& buffer, HttpRequest* request) {
    
    return parseHttpRequest(buffer, request);
}


void closeConnection(socket_t fd) {
    std::lock_guard<std::mutex> lock(g_connMutex);
    auto it = g_tcpIpConnections.find(fd);
    if (it != g_tcpIpConnections.end()) {
        CLOSE_SOCKET(fd);
        g_tcpIpConnections.erase(it);
    }
}

void handleAccept(socket_t listenSock) {

    while (true) {

        socket_t client = accept(listenSock, nullptr, nullptr);

        if (client == INVALID_SOCKET) {
            int err = GET_SOCKET_ERROR();
            if (err == WOULDBLOCK_ERROR)
                break;
            std::cerr << "accept error: " << err << "\n";
            break;
        }

        SET_NONBLOCKING(client);

        g_tcpIpConnections.emplace(client, TcpIpConnection{ client });

        std::cout << "Accepted client " << client << "\n";
    }
}

void handleRead(socket_t fd) {
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
            int err = GET_SOCKET_ERROR();
            if (err == WOULDBLOCK_ERROR)
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
            if (it == g_tcpIpConnections.end()) {
                delete req;
                return;
            }

            if (!tryParseHttpRequest(it->second.readBuf, req)) {
                delete req;
                break;
            }
        }
        req->fd = fd;

        placeHttpRequest(req);
    }
}   

void placeHttpRequest(HttpRequest* req)
{
    if (g_requestHandlers.empty()) {
        std::cerr << "Error: No request handlers available!\n";
        delete req; 
        return;
    }

    static size_t placeId = 0;
    g_requestHandlers[placeId]->pushHttpRequest(req);

    placeId = (placeId + 1) % g_requestHandlers.size();
}

void handleWrite(socket_t fd) {

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
        int err = GET_SOCKET_ERROR();
        if (err != WOULDBLOCK_ERROR) {
            std::cerr << "send error " << err << " on " << fd << "\n";
            CLOSE_SOCKET(fd);
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

void tcpServerLoop(socket_t listenSock) {

	while (g_running) {

		std::vector<pollfd_t> fileDescriptors;

		pollfd_t lf{};
		lf.fd = listenSock;
		lf.events = POLLRDNORM;
		fileDescriptors.push_back(lf);

		for (auto& kv : g_tcpIpConnections) {
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

int main() {
    g_router.setRoutes();

    g_tcpIpListener = TcpIpListener::getInstance();
    g_tcpIpListener->init();

    if (!g_tcpIpListener->isListening)
    {
        cerr << "TcpIpListener failed!\n";
    }

    unsigned int nrOfProcessors = get_processor_count();

    std::vector<std::thread> workers;

    for (int i = 0; i < nrOfProcessors; ++i)
    {
        RequestHandler* requestHandler = new RequestHandler();
        workers.emplace_back(&RequestHandler::run, requestHandler);
        g_requestHandlers.push_back(requestHandler);
    }
        
    tcpServerLoop(g_tcpIpListener->getListenSocket());

    g_running = false;
    g_requestQueue.stop();
    g_responseQueue.stop();

    for (auto& t : workers)
        t.join();

    for (RequestHandler* handler : g_requestHandlers) {
        delete handler;
    }
    g_requestHandlers.clear();

    g_tcpIpListener->release();
    return 0;
}
