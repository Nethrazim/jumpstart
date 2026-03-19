#pragma once

#include <string>
#include "platform.h"


struct TcpIpConnection {
	socket_t fd = INVALID_SOCKET;
	std::string readBuf;
	std::string writeBuf;
	bool wantWrite = false;
};