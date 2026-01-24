#include "AbstractClient.h"

Client::AbstractClient::AbstractClient(int domain, int service, int protocol, int port, u_long network_interface)
	:
	conSocket(std::make_unique<sockets::ConnectingSocket>
		(domain, service, protocol, port, network_interface))
{
	std::cout << "client interface ctor" << std::endl;
	conSocket->startConnect();
}

Client::AbstractClient::~AbstractClient()
{
	conSocket->stopConnection();
}


