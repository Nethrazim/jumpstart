#pragma once

#include "platform.h"
#include <string>

struct TcpIpConnection {
	socket_t fd = INVALID_SOCKET;
	std::string readBuf;
	std::string writeBuf;
	bool wantWrite = false;
};