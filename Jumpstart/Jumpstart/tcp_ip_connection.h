#pragma once

#include <string>
#include "platform.h"


struct TcpIpConnection {
	socket_t fd = INVALID_SOCKET;

	std::string readBuf;
	std::string writeBuf;

	bool wantWrite = false;

	bool isHttpLineParsed = false;
	bool isHeadersParsed = false;
	bool isBodyParsed = false;

	HttpRequest* currentRequest = nullptr;
	void resetParsingState()
	{
		isHttpLineParsed = isHeadersParsed = isBodyParsed = false;
	}

	bool isCurrentRequestCompleted()
	{
		std::string& method = currentRequest->method;

		if (method == "GET"
			|| method == "DELETE"
			|| method == "HEAD")
		{
			return isHttpLineParsed && isHeadersParsed;
		}
		else
		{
			return isHttpLineParsed && isHeadersParsed;
		}
	}
};