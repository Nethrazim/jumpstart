#include <winsock2.h>
#include <unordered_map>

#include "tcp_ip_connection.h"
#include "blocking_queue.h"
#include "http_request.h"
#include "http_response.h"

class TcpIpHandler;

std::unordered_map<SOCKET, TcpIpConnection> g_tcpIpConnections;
BlockingQueue<HttpRequest> g_requestQueue;
BlockingQueue<HttpResponse> g_responseQueue;
std::vector<std::thread> workers;
std::vector<TcpIpHandler*> g_tcpIpHandlers;