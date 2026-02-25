#pragma once

#include <WinSock2.h>
#include <string>

struct TcpIpConnection {
	SOCKET fd = INVALID_SOCKET;
	std::string readBuf;
	std::string writeBuf;
	bool wantWrite = false;
};