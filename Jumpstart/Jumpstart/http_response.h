#pragma once

#include <WinSock2.h>
#include <string>
#include <map>

using std::map;
using std::string;

struct HttpResponse
{
	SOCKET fd = INVALID_SOCKET;
	map<string, string> headers;
	std::string raw;
};

struct Response 
{
	map<string, string> headers;
	std::string raw;
};