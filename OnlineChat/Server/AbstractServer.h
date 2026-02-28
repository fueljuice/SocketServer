#pragma once
#include "../Sockets/ListeningSocket.h"

namespace sockets::server
{	
class AbstractServer
{

public:
	AbstractServer(int domain, int service, int protocol,
		int port, u_long network_interaface, int backlog);


	//  derived class must have provide launching and stopping functions
	virtual void launch() = 0;
	virtual void stop() = 0;

protected:
	// a listening socket member to handle the listening accepting and closing of the server socket
	std::unique_ptr<ListeningSocket> lstnSocket;

};	
}