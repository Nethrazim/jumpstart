#pragma once

#include "platform.h"
#include <string>
#include <map>

using std::map;
using std::string;

struct HttpResponse
{
	socket_t fd = INVALID_SOCKET;
	map<string, string> headers;
	std::string raw;
};

struct Response
{
	map<string, string> headers;
	std::string raw;
};