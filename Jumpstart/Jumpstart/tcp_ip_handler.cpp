#include "tcp_ip_handler.h"
#include "http_response.h"
#include "router.h"
#include "blocking_queue.h"

extern Router g_router;
extern BlockingQueue<HttpResponse> g_responseQueue;

void TcpIpHandler::pushHttpRequest(HttpRequest* req)
{
	requestQueue_.emplace(req);
}

void TcpIpHandler::run()
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