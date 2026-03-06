#include "tcp_ip_listener.h"

#include <iostream>
using std::cerr; using std::cout; using std::endl;

TcpIpListener::TcpIpListener() {
	isInitialized = false;
	isBound = false;
	isListening = false;
	listenSocket = INVALID_SOCKET;
	memset(&wsa,0, sizeof(wsa));
}

bool TcpIpListener::init(int port, bool bindAndListen) {

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cerr << "WSAStartup failed\n";
		return 1;
	}

	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (listenSocket == INVALID_SOCKET) {
		cerr << "socket failed\n";
		isInitialized = false;
		return false;
	}

	//set non blocking mode
	u_long mode = 1;
	ioctlsocket(listenSocket, FIONBIO, &mode);
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	isInitialized = true;

	if (bindAndListen)
	{
		if (bind(listenSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
			int err = WSAGetLastError();
			cerr << "Socket bind failed, error: " << err << endl;
			isBound = false;
			return false;

		}
		isBound = true;


		if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
			int err = WSAGetLastError();
			cerr << "Socket listen failed, error: " << err << endl;
			isListening = false;
			return false;
		}
		isListening = true;

		cout << "Listening on port " << port << endl;
	}

	return true;
}

bool TcpIpListener::socketBind() {
	
	if (!isInitialized)
	{
		cerr << "TcpIp Listener not initialized\n";
		return false;
	}

	if (bind(listenSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
	
		cerr << "Socket bind failed\n";
		isBound = false;
		return false;
	}

	isBound = true;
	return isBound;
}

bool TcpIpListener::socketListen()
{
	if (!(isInitialized && isBound))
	{
		cerr << "TcpIp Listener not initialized and bound\n";
		isListening = false;
		return isListening;
	}

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cerr << "Socket listen failed\n";
		isListening = false;
		return isListening;
	}

	isListening = true;
	cout << "Listening on port " << addr.sin_port << endl;

	return isListening;
}

SOCKET& TcpIpListener::getListenSocket() {
	return listenSocket;
}

TcpIpListener* TcpIpListener::instance_ = nullptr;
TcpIpListener* TcpIpListener::getInstance() {
	if (!instance_) {
		instance_ = new TcpIpListener();
	}

	return instance_;
}

void TcpIpListener::release() {
	if (listenSocket != SOCKET_ERROR)
	{
		closesocket(listenSocket);
	}
	
	WSACleanup();
}

TcpIpListener::~TcpIpListener() {
	
}

