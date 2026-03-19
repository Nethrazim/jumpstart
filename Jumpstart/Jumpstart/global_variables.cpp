#include <vector>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <mutex>

#include "platform.h"
#include "app-router.h"
#include "http_request.h"
#include "http_response.h"
#include "blocking_queue.h"
#include "tcp_ip_connection.h"

class RequestHandler;
class TcpIpListener;

AppRouter g_router;
std::mutex g_connMutex;
std::vector<std::thread> g_workers;
std::atomic<bool> g_running{ true };
TcpIpListener* g_tcpIpListener = nullptr;
BlockingQueue<HttpRequest> g_requestQueue;
BlockingQueue<HttpResponse> g_responseQueue;
std::vector<RequestHandler*> g_requestHandlers;
std::unordered_map<socket_t, TcpIpConnection> g_tcpIpConnections;






