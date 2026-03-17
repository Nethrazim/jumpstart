#pragma once

#include "platform.h"
#include "http_request.h"

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
	void release();
	void drainWorkerResponses();
	void placeHttpRequest(HttpRequest* req);
	bool tryParseHttpRequest(std::string& buffer, HttpRequest* request);

	void handleRead(socket_t fd);
	void handleWrite(socket_t fd);
	void handleAccept(socket_t listenSock);
	void closeConnection(socket_t fd);

private:
	socket_t listenSocket;
	sockaddr_in addr{};
	TcpIpListener();
	static TcpIpListener* instance_;
	~TcpIpListener();
};