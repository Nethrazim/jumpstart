#include <map>
#include <string>
#include <iostream>

#include "http_request.h"

using std::string;
using std::map;
using std::cout;

bool extractBodyAndHeader(std::string& clonedBuffer, std::string& headersBuffer, std::string& bodyBuffer);
bool mapHttpLine(std::string& headersBuffer, HttpRequest* request);
void mapHeaders(std::string& headersBuffer, HttpRequest* request);


bool parseHttpRequest(std::string& buffer, HttpRequest* request)
{
	
	if (buffer == "")
		return false;


	std::string clonedBuffer = buffer;
	
	
	std::string headersBuffer = "";
	std::string bodyBuffer = "";

	if (!extractBodyAndHeader(clonedBuffer, headersBuffer, bodyBuffer))
	{
		return false;
	}

	if (!mapHttpLine(headersBuffer, request))
	{
		return false;
	}

	mapHeaders(headersBuffer, request);
	
	int contentLength = 0;

	if (request->headers.find("Content-Length") != request->headers.end())
	{
		contentLength = std::stoi(request->headers.find("Content-Length")->second);
	}

	if (contentLength > 0 && bodyBuffer.length() < contentLength)
	{
		return false;
	}

	request->body = bodyBuffer;

	//todo CHECK IF BODY EXISTS ETC ETC ETC
	if (request->method == "POST" )
	{
		cout << "ITS A POST METHOD" << endl;
	}


	
	//TODO check this because parseHttpRequest at some 
	//point runs useless
	buffer = "";
	return true;
}

bool extractBodyAndHeader(std::string& clonedBuffer, std::string& headersBuffer, std::string& bodyBuffer) {

	size_t pos = clonedBuffer.find("\r\n\r\n");
	if (pos == std::string::npos)
	{
		return false;
	}

	if (clonedBuffer.length() < pos + 4) {
		return false;
	}

	headersBuffer = clonedBuffer.substr(0, pos);
	clonedBuffer.erase(0, pos + 4);

	bodyBuffer = clonedBuffer;	
	return true;
}

bool mapHttpLine(std::string& headersBuffer, HttpRequest* request)
{
	size_t pos = headersBuffer.find("\r\n");
	if (pos == std::string::npos)
	{
		return false;
	}

	std::string requestLine = headersBuffer.substr(0, pos);
	size_t mEnd = requestLine.find(' ');
	if (mEnd == std::string::npos) return false;

	size_t pEnd = requestLine.find(' ', mEnd + 1);
	if (pEnd == std::string::npos) return false;

	request->method = requestLine.substr(0, mEnd);
	request->path = requestLine.substr(mEnd + 1, pEnd - (mEnd + 1));
	request->body.clear();

	headersBuffer.erase(0, pos + 2);
	return true;
}

void mapHeaders(std::string& headersBuffer, HttpRequest* request) {

	while (!headersBuffer.empty())
	{
		size_t pos = headersBuffer.find("\r\n"); //detect each line of the HTTP HEADERS 

		if (pos == std::string::npos) {
			break;
		}

		if (pos == 0) {
			headersBuffer.erase(0, 2);
			break;
		}

		std::string headerLine = headersBuffer.substr(0, pos);

		size_t colonPos = headerLine.find(':');
		if (colonPos != std::string::npos) {
			std::string headerName = headerLine.substr(0, colonPos);
			std::string headerValue = headerLine.substr(colonPos + 1);

			if (!headerValue.empty() && headerValue[0] == ' ') {
				headerValue = headerValue.substr(1);
			}
			request->headers[headerName] = headerValue;
		}

		headersBuffer.erase(0, pos + 2);
	}
}