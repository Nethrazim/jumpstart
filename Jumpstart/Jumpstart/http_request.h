#pragma once

#include <map>
#include <string>
#include <iostream>
#include "platform.h"

using std::cout; using std::endl;
using std::string;
using std::map;

struct HttpRequest
{
	string originalBuffer;
	socket_t fd = INVALID_SOCKET;
	string method;
	string path;
	string body;
	map<string, string> headers;

	//Default constructor
	HttpRequest() {
		cout << "HttpRequest() default - " << this << std::endl;
	}

	//Copy constructor
	HttpRequest(const HttpRequest& other)
		: fd(other.fd), method(other.method), path(other.path), body(other.body) {

		cout << "HttpRequest(const&) COPY from " << &other << " to " << this << std::endl;
	}

	//Move constructor
	HttpRequest(HttpRequest&& other) noexcept 
		: fd(other.fd), method(std::move(other.method)), path(std::move(other.path)), body(std::move(other.body)), headers(std::move(other.headers))
	{			
		cout << "HttpRequest(&&) MOVE from " << &other << " to " << this << std::endl;
		other.fd = INVALID_SOCKET;				
	}

	~HttpRequest() {
		cout << "~HttpRequest() destruct - " << this << std::endl;
	}
};

struct Request 
{
	string method;
	string path;
	string body;
	map<string, string> headers;
};