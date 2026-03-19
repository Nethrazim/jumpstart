#pragma once

#include <map>
#include <string>
#include "platform.h"

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