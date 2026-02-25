#pragma once

#include <WinSock2.h>
#include <string>

struct HttpResponse 
{
	SOCKET fd = INVALID_SOCKET;
	std::string raw;
};