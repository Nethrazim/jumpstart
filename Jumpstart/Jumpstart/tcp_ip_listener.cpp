#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include <cstring>
#include <iostream>
#include <unordered_map>


#include "platform.h"
#include "http_response.h"
#include "blocking_queue.h"
#include "request_handler.h"
#include "tcp_ip_listener.h"
#include "tcp_ip_connection.h"


using std::cerr; using std::cout; using std::endl;


extern std::atomic<bool> g_running;
extern BlockingQueue<HttpResponse> g_responseQueue;
extern std::mutex g_connMutex;
extern std::vector<RequestHandler*> g_requestHandlers;

bool parseHttpRequest(std::string&, HttpRequest* out);

TcpIpListener::TcpIpListener() {
	isInitialized = false;
	isBound = false;
	isListening = false;
	listenSocket = INVALID_SOCKET;
}

bool TcpIpListener::init(int port, bool bindAndListen) {

	if (!Platform::initialize()) {
		std::cerr << "Platform initialization failed\n";
		return false;
	}

	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listenSocket == INVALID_SOCKET) {
		cerr << "socket failed\n";
		isInitialized = false;
		return false;
	}

	// Set non-blocking mode
	SET_NONBLOCKING(listenSocket);

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	isInitialized = true;

	if (bindAndListen)
	{
		if (bind(listenSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
			int err = GET_SOCKET_ERROR();
			cerr << "Socket bind failed, error: " << err << endl;
			isBound = false;
			return false;

		}
		isBound = true;


		if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
			int err = GET_SOCKET_ERROR();
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

socket_t& TcpIpListener::getListenSocket() {
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
	if (listenSocket != INVALID_SOCKET)
	{
		CLOSE_SOCKET(listenSocket);
	}

	Platform::cleanup();
}

RequestHandler* TcpIpListener::pickRequestHandler() {

	static size_t pickId = 0;

	pickId = (pickId + 1) % g_requestHandlers.size();

	return g_requestHandlers.at(pickId);
}

void TcpIpListener::handleAccept(socket_t listenSock) {

	while (true) {

		socket_t client = accept(listenSock, nullptr, nullptr);

		if (client == INVALID_SOCKET) {
			int err = GET_SOCKET_ERROR();
			if (err == WOULDBLOCK_ERROR)
				break;
			std::cerr << "accept error: " << err << "\n";
			break;
		}

		SET_NONBLOCKING(client);

		pickRequestHandler()->tcpIpConnections_.emplace(client, TcpIpConnection{ client });

		std::cout << "Accepted client " << client << "\n";
	}
}

void TcpIpListener::placeHttpRequest(HttpRequest* req)
{
	if (g_requestHandlers.empty()) {
		std::cerr << "Error: No request handlers available!\n";
		delete req;
		return;
	}

	static size_t placeId = 0;
	g_requestHandlers[placeId]->pushHttpRequest(req);

	placeId = (placeId + 1) % g_requestHandlers.size();
}


void TcpIpListener::tcpServerLoop(socket_t listenSock) {
	
	while (g_running) {
		//TODO eliminate this fileDescriptor 
		//just to hold the listenSock
		std::vector<pollfd_t> fileDescriptors;

		pollfd_t lf{};
		lf.fd = listenSock;
		lf.events = POLLRDNORM;

		fileDescriptors.push_back(lf);

		int n = POLL(fileDescriptors.data(), fileDescriptors.size(), 100);

		if (n == SOCKET_ERROR) {
			int err = GET_SOCKET_ERROR();
			std::cerr << "Poll error: " << err << "\n";
			continue;
		}

		for (auto& p : fileDescriptors) {
			if (p.revents & POLLRDNORM)
				handleAccept(listenSock);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

TcpIpListener::~TcpIpListener() {

}

