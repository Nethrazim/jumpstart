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

extern AppRouter g_router;
extern std::atomic<bool> g_running;
extern TcpIpListener* g_tcpIpListener;
extern BlockingQueue<HttpRequest> g_requestQueue;
extern BlockingQueue<HttpResponse> g_responseQueue;
extern std::vector<RequestHandler*> g_requestHandlers;
extern std::unordered_map<socket_t, TcpIpConnection> g_tcpIpConnections;


int main() {
    g_router.setRoutes();

    g_tcpIpListener = TcpIpListener::getInstance();
    g_tcpIpListener->init();

    if (!g_tcpIpListener->isListening)
    {
        cerr << "TcpIpListener failed!\n";
    }

    unsigned int nrOfProcessors = get_processor_count();

    //TODO move this
    std::vector<std::thread> workers;

    for (int i = 0; i < nrOfProcessors; ++i)
    {
        RequestHandler* requestHandler = new RequestHandler();
        workers.emplace_back(&RequestHandler::run, requestHandler);
        g_requestHandlers.push_back(requestHandler);
    }
        
    g_tcpIpListener->tcpServerLoop(g_tcpIpListener->getListenSocket());

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
