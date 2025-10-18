#pragma once
#include "ListeningSocket.h"

namespace HDE
{
class SocketServer
{

private:
	std::unique_ptr<ListeningSocket> lstnSocket;

	virtual void acceptConnection() = 0;

	virtual void handleConnection() = 0;

	virtual void responde() = 0;

public:
	SocketServer(int domain, int service, int protocol, 
		int port, u_long network_interaface, int backlog);

	virtual void launch() = 0;

	// rule of five
	virtual ~SocketServer() = default;
	SocketServer(const SocketServer&) = delete;  // unique_ptr  noncopyable
	SocketServer& operator=(const SocketServer&) = delete;


};
}