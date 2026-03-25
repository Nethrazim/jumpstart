#include <map>
#include <string>
#include <iostream>

#include "http_request.h"
#include "tcp_ip_connection.h"

using std::string;
using std::map;
using std::cout;

bool extractBodyAndHeader(std::string& clonedBuffer, std::string& headersBuffer, std::string& bodyBuffer);
bool mapHttpLine(std::string& headersBuffer, HttpRequest* request);
void mapHeaders(std::string& headersBuffer, HttpRequest* request);


bool parseHttpRequest(std::string& buffer, HttpRequest* request, TcpIpConnection& tcpIpConnection)
{
	
	if (buffer == "")
		return false;

	request->buffer = buffer;
	
	
	std::string& headersBuffer = request->headersBuffer;
	std::string& bodyBuffer = request->bodyBuffer;

	//if (!tcpIpConnection.isHeadersParsed)
	//{
		if (!extractBodyAndHeader(request->buffer, headersBuffer, bodyBuffer))
		{
			return false;
		}
	//}

	
//	if (!tcpIpConnection.isHttpLineParsed)
//	{
		if (!mapHttpLine(headersBuffer, request))
		{
			return false;
		}

		tcpIpConnection.isHttpLineParsed = true;
	//}
	

	mapHeaders(headersBuffer, request);
	//tcpIpConnection.isHeadersParsed = true;

	//if (!tcpIpConnection.isBodyParsed)
	//{
		int contentLength = 0;
		bool isChunkedEncoding = false;


		if (request->headers.find("Content-Length") != request->headers.end())
		{
			//is not chunked encoding
			contentLength = std::stoi(request->headers.find("Content-Length")->second);
			isChunkedEncoding = false;
		}
		else if (request->headers.find("Transfer-Encoding") != request->headers.end())
		{
			isChunkedEncoding = true;
		}

		if (!isChunkedEncoding && bodyBuffer.length() < contentLength)
		{
			return false;
		}

		if (isChunkedEncoding && bodyBuffer.find("0\r\n\r\n") == std::string::npos)
		{
			return false;
		}

		if (request->method == "POST"
			|| request->method == "PUT"
			|| request->method == "PATCH") {
			
		//	tcpIpConnection.isBodyParsed = true;
			request->body = bodyBuffer;
		}

		

		
	//}
	

	tcpIpConnection.resetParsingState();

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