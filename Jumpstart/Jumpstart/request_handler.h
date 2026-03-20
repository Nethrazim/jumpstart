#pragma once

#include <queue>
#include "http_request.h"
#include "http_response.h"

class TcpIpConnection;

class RequestHandler
{

public:
	bool isAlocated = false;

	void pushHttpRequest(HttpRequest* req);

	void loop();

	std::unordered_map<socket_t, TcpIpConnection> tcpIpConnections_;

private:
    std::queue<HttpRequest*> requestQueue_;
	std::queue<HttpResponse> responseQueue_;

	void handleRead(socket_t fd);
	void handleWrite(socket_t fd);
	void closeConnection(socket_t fd);
	void drainWorkerResponses();
};

