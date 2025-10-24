#include "ListeningSocket.h"


HDE::ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long network_interface, int bklog)
	: 
	SimpleSocket(domain, service, protocol, port, network_interface),
	backlog(bklog)
{
	connectToNetwork(getSock(), getAddress());
}

int HDE::ListeningSocket::connectToNetwork(int sock, sockaddr_in address)
{
	bindSocket(sock, address);
	//testing binding
	testConnection(getSock());
	std::cout << "binded sucsessfuly to port" << getAddress().sin_port << std::endl;

	//testing listening
	startLisetning();
	testConnection(listening);
	std::cout << "started lisetning sucsessfuly on port" << getAddress().sin_port << std::endl;
}

int HDE::ListeningSocket::bindSocket(int sock, sockaddr_in address)
{
	return bind(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address));
}

void HDE::ListeningSocket::startLisetning()
{
	listening = listen(getSock(), backlog);
}
