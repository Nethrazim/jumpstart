#pragma once

#include <WinSock2.h>
#include <string>
#include <map>

using std::string;
using std::map;

struct HttpRequest
{
	SOCKET fd = INVALID_SOCKET;
	string method;
	string path;
	string body;
	map<string, string> headers;
};