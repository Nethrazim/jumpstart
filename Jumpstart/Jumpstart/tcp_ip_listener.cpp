#include "tcp_ip_listener.h"
#include "platform.h"

#include <iostream>
#include <cstring>
#include "http_response.h"
#include <mutex>
#include <vector>
#include <unordered_map>
#include "blocking_queue.h"
#include "tcp_ip_connection.h"
#include "request_handler.h"

using std::cerr; using std::cout; using std::endl;


extern std::atomic<bool> g_running;
extern BlockingQueue<HttpResponse> g_responseQueue;
extern std::unordered_map<socket_t, TcpIpConnection> g_tcpIpConnections;
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

void TcpIpListener::drainWorkerResponses() {
	HttpResponse resp;
	while (g_responseQueue.try_pop(resp)) {
		if (resp.fd == INVALID_SOCKET)
			continue;
		std::lock_guard<std::mutex> lock(g_connMutex);
		auto it = g_tcpIpConnections.find(resp.fd);
		if (it == g_tcpIpConnections.end())
			continue;
		it->second.writeBuf += resp.raw;
		it->second.wantWrite = true;
	}
}

void TcpIpListener::closeConnection(socket_t fd) {
	std::lock_guard<std::mutex> lock(g_connMutex);
	auto it = g_tcpIpConnections.find(fd);
	if (it != g_tcpIpConnections.end()) {
		CLOSE_SOCKET(fd);
		g_tcpIpConnections.erase(it);
	}
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

		g_tcpIpConnections.emplace(client, TcpIpConnection{ client });

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

bool TcpIpListener::tryParseHttpRequest(std::string& buffer, HttpRequest* request) {

	return parseHttpRequest(buffer, request);
}


void TcpIpListener::handleRead(socket_t fd) {
	char buf[4096];

	while (true) {
		int n = recv(fd, buf, sizeof(buf), 0);
		if (n > 0) {
			std::lock_guard<std::mutex> lock(g_connMutex);
			auto it = g_tcpIpConnections.find(fd);
			if (it == g_tcpIpConnections.end())
				return;
			it->second.readBuf.append(buf, n);
		}
		else if (n == 0) {
			std::cout << "Client closed " << fd << "\n";
			closeConnection(fd);
			return;
		}
		else {
			int err = GET_SOCKET_ERROR();
			if (err == WOULDBLOCK_ERROR)
				break;
			std::cerr << "recv error " << err << " on " << fd << "\n";
			closeConnection(fd);
			return;
		}
	}

	while (true) {
		HttpRequest* req = new HttpRequest();
		{
			std::lock_guard<std::mutex> lock(g_connMutex);
			auto it = g_tcpIpConnections.find(fd);
			if (it == g_tcpIpConnections.end()) {
				delete req;
				return;
			}

			if (!tryParseHttpRequest(it->second.readBuf, req)) {
				delete req;
				break;
			}
		}
		req->fd = fd;

		placeHttpRequest(req);
	}
}


void TcpIpListener::handleWrite(socket_t fd) {

	std::lock_guard<std::mutex> lock(g_connMutex);

	auto it = g_tcpIpConnections.find(fd);

	if (it == g_tcpIpConnections.end())
		return;

	TcpIpConnection& conn = it->second;

	if (conn.writeBuf.empty()) {
		conn.wantWrite = false;
		return;
	}

	int n = send(fd, conn.writeBuf.data(), (int)conn.writeBuf.size(), 0);

	if (n > 0) {
		conn.writeBuf.erase(0, n);
		if (conn.writeBuf.empty())
			conn.wantWrite = false;
	}
	else {
		int err = GET_SOCKET_ERROR();
		if (err != WOULDBLOCK_ERROR) {
			std::cerr << "send error " << err << " on " << fd << "\n";
			CLOSE_SOCKET(fd);
			g_tcpIpConnections.erase(it);
		}
	}
}


void TcpIpListener::tcpServerLoop(socket_t listenSock) {
	while (g_running) {

		std::vector<pollfd_t> fileDescriptors;

		pollfd_t lf{};
		lf.fd = listenSock;
		lf.events = POLLRDNORM;
		fileDescriptors.push_back(lf);

		for (auto& kv : g_tcpIpConnections) {
			pollfd_t pFd{};
			pFd.fd = kv.first;
			pFd.events = POLLRDNORM;
			if (kv.second.wantWrite) {
				pFd.events |= POLLWRNORM;
			}
			fileDescriptors.push_back(pFd);
		}

		int n = POLL(fileDescriptors.data(), fileDescriptors.size(), 100);

		if (n == SOCKET_ERROR) {
			int err = GET_SOCKET_ERROR();
			std::cerr << "Poll error: " << err << "\n";
			continue;
		}

		for (auto& p : fileDescriptors) {
			if (p.fd == listenSock) {
				if (p.revents & POLLRDNORM)
					handleAccept(listenSock);
			}
			else {
				if (p.revents & POLLRDNORM)
					handleRead(p.fd);
				if (p.revents & POLLWRNORM)
					handleWrite(p.fd);
				if (p.revents & (POLLERR | POLLHUP | POLLNVAL))
					closeConnection(p.fd);
			}
		}

		drainWorkerResponses();
	}
}

TcpIpListener::~TcpIpListener() {

}

