#include "ClientInterface.h"

Client::ClientInterface::ClientInterface(int domain, int service, int protocol, int port, u_long network_interface)
	:
	conSocket(std::make_unique<sockets::ConnectingSocket>
		(domain, service, protocol, port, network_interface))
{
	std::cout << "client interface ctor" << std::endl;
	conSocket->startConnect();
}

Client::ClientInterface::~ClientInterface()
{
	conSocket->stopConnection();
}


