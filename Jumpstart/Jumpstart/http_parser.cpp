#pragma once

#include <string>
#include "http_request.h"


bool tryParseHttpRequest(std::string& buffer, HttpRequest* out) {
	std::size_t pos = buffer.find("\r\n\r\n");

	if (pos == std::string::npos) {
		return false;
	}

	std::string headerPart = buffer.substr(0, pos);
	
	//First Line: METHOD PATH HTTP/
	std::size_t lineEnd = headerPart.find("\r\n");
	if (lineEnd == std::string::npos) {
		return false;
	}

	std::string requestLine = headerPart.substr(0, lineEnd);
	std::size_t mEnd = requestLine.find(' ');
	if (mEnd == std::string::npos) return false;
	std::size_t pEnd = requestLine.find(' ', mEnd + 1);
	if (pEnd == std::string::npos) return false;

	out->method = requestLine.substr(0, mEnd);
	out->path = requestLine.substr(mEnd + 1, pEnd - (mEnd + 1));
	out->body.clear(); // ignoring body

	return true;
}