#pragma once

#include <WinSock2.h>
#include <string>

struct HttpRequest
{
	SOCKET fd = INVALID_SOCKET;
	std::string method;
	std::string path;
	std::string body;
};