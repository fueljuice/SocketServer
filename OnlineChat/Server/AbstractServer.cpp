#include "AbstractServer.h"

sockets::server::AbstractServer::AbstractServer(int domain, int service,
	int protocol, int port, u_long network_interaface, int backlog)

	: lstnSocket(std::make_unique<ListeningSocket>(domain
		, service, protocol, port, network_interaface, backlog))
{
	std::cout << "init SocketServer" << std::endl;
}

