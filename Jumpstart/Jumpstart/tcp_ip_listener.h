#pragma once

#include "winsock2.h"

class TcpIpListener
{
public:
	bool isInitialized = false;
	bool isBound = false;
	bool isListening = false;

	bool init(int port = 8008, bool bindAndListen = true);
	bool socketBind();
	bool socketListen();

	static TcpIpListener* getInstance();

	SOCKET& getListenSocket();

	void release();
private:
	WSADATA wsa;
	SOCKET listenSocket;
	sockaddr_in addr{};
	TcpIpListener();
	static TcpIpListener* instance_;
	~TcpIpListener();
};