#include "request_handler.h"
#include "http_response.h"
#include "app-router.h"
#include "blocking_queue.h"

extern AppRouter g_router;
extern BlockingQueue<HttpResponse> g_responseQueue;

void RequestHandler::pushHttpRequest(HttpRequest* req)
{
	requestQueue_.emplace(req);
}

void RequestHandler::run()
{
    while (isRunning) {
        HttpRequest* req = nullptr;
       
        if (!requestQueue_.empty()) {
            req = requestQueue_.front();
            requestQueue_.pop();
        } else {
            continue; // Skip iteration if queue is empty
        }

        if (!isRunning || req->fd == INVALID_SOCKET)
            break;

        HttpResponse resp;
        resp.fd = req->fd;

        g_router.dispatch(*req, resp);

        g_responseQueue.push(std::move(resp));
    }
}