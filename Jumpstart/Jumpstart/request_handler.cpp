#include <atomic>

#include "app-router.h"
#include "http_response.h"
#include "blocking_queue.h"
#include "request_handler.h"



extern AppRouter g_router;
extern BlockingQueue<HttpResponse> g_responseQueue;
extern std::atomic<bool> g_running;

void RequestHandler::pushHttpRequest(HttpRequest* req)
{
	requestQueue_.emplace(req);
}

void RequestHandler::run()
{
    while (g_running) {
        HttpRequest* req = nullptr;
       
        if (!requestQueue_.empty()) {
            req = requestQueue_.front();
            requestQueue_.pop();
        } else {
            continue;
        }

        if (!isRunning || req->fd == INVALID_SOCKET)
            break;

        HttpResponse resp;
        resp.fd = req->fd;

        g_router.dispatch(*req, resp);

        g_responseQueue.push(std::move(resp));

        delete req; 
    }
}