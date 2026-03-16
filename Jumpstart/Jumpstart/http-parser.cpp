#include <string>
#include "http_request.h"
#include <map>
using std::string;
using std::map;

bool extractHeader(std::string& buffer, std::string& headersBuffer);
bool mapHttpLine(std::string& headersBuffer, HttpRequest* request);


bool parseHttpRequest(std::string& buffer, HttpRequest* request)
{
	
	if (buffer == "")
		return false;

	std::string clonedBuffer = buffer;
	
	std::string headersBuffer = "";
	map<string, string>* mappedHeaders;

	if (!extractHeader(clonedBuffer, headersBuffer))
	{
		return false;
	}

	if (!mapHttpLine(headersBuffer, request))
	{
		return false;
	}
	
	//TODO check this because parseHttpRequest at some 
	//point runs useless
	buffer = "";
	return true;
}

bool extractHeader(std::string& buffer, std::string& headersBuffer) {
	size_t pos = buffer.find("\r\n\r\n");
	if (pos == std::string::npos)
	{
		return false;
	}

	if (buffer.length() < pos + 4) {
		return false;
	}

	headersBuffer = buffer.substr(0, pos);
	buffer.erase(0, pos + 4);
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

bool mapHeaders(std::string& headersBuffer, HttpRequest* request) {
	return true;
}