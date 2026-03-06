#pragma once

#include <queue>
#include "http_request.h"

class TcpIpHandler
{

public:
	bool isAlocated = false;
	bool isRunning = true;

	void pushHttpRequest(HttpRequest& req);

	void run();

private:
    std::queue<HttpRequest> requestQueue_;
};

