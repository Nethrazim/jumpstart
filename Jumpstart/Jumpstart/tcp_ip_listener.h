#pragma once

#include "platform.h"
#include "http_request.h"
#include "request_handler.h"

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
	socket_t& getListenSocket();
	void tcpServerLoop(socket_t listenSock);
	void handleAccept(socket_t listenSock);
	void release();

	void placeHttpRequest(HttpRequest* req);

	RequestHandler* pickRequestHandler();

private:
	socket_t listenSocket;
	sockaddr_in addr{};

	TcpIpListener();
	static TcpIpListener* instance_;

	~TcpIpListener();
};