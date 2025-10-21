#pragma once
#include "ListeningSocket.h"

namespace HDE
{

struct clientSocketData
{
	int clientSocket;
	struct sockaddr clientAddr;
	char buffer[30000];
	clientSocketData(int socket, struct sockaddr clientAddr)
		: clientSocket(socket), clientAddr(clientAddr) {};
};


class SocketServer
{

private:
	// accepts a socket and returns a new socket that was created by the accept function
	virtual clientSocketData acceptConnection() = 0;

	virtual void handleConnection() = 0;

	virtual void responde() = 0;

protected:
	std::unique_ptr<ListeningSocket> lstnSocket;


public:
	SocketServer(int domain, int service, int protocol, 
		int port, u_long network_interaface, int backlog);

	virtual void launch() = 0;

	// rule of three
	virtual ~SocketServer() = default;
	SocketServer(const SocketServer&) = delete;  // unique_ptr  noncopyable
	SocketServer& operator=(const SocketServer&) = delete;


};
}