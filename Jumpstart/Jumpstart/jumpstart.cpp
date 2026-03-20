#include <mutex>
#include <queue>
#include <vector>
#include <thread>
#include <atomic>
#include <string>
#include <iostream>
#include <unordered_map>
#include <condition_variable>

#include "platform.h"
#include "app-router.h"
#include "http_request.h"
#include "http_response.h"
#include "blocking_queue.h"
#include "tcp_ip_listener.h"
#include "request_handler.h"
#include "tcp_ip_connection.h"


using std::cerr; 

extern AppRouter g_router;
extern std::atomic<bool> g_running;
extern TcpIpListener* g_tcpIpListener;
extern std::vector<std::thread> g_workers;
extern std::vector<RequestHandler*> g_requestHandlers;


void spawnTcpHandlers() {
    unsigned int nrOfProcessors = get_processor_count();

    for (int i = 0; i < nrOfProcessors; ++i) {
        RequestHandler* requestHandler = new RequestHandler();
        g_requestHandlers.push_back(requestHandler);
        g_workers.emplace_back(&RequestHandler::loop, requestHandler);
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

    spawnTcpHandlers();

    g_tcpIpListener->tcpServerLoop(g_tcpIpListener->getListenSocket());

    g_running = false;

    for (auto& t : g_workers)
        t.join();

    for (RequestHandler* handler : g_requestHandlers) {
        delete handler;
    }

    g_requestHandlers.clear();
    g_tcpIpListener->release();

    return 0;
}