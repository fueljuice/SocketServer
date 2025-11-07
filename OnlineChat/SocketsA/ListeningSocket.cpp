#include "ListeningSocket.h"


HDE::ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long network_interface, int bklog)
	: 
	SimpleSocket(domain, service, protocol, port, network_interface),
	backlog(bklog)
{
	connectToNetwork(getSock(), getAddress());
}

void HDE::ListeningSocket::connectToNetwork(SOCKET sock, sockaddr_in address)
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

SOCKET HDE::ListeningSocket::bindSocket(SOCKET sock, sockaddr_in address)
{
	return bind(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address));
}

void HDE::ListeningSocket::startLisetning()
{
	listening = listen(getSock(), backlog);
}

void HDE::ListeningSocket::stopLisetning()
{
	SOCKET s = getSock();
	if (s != INVALID_SOCKET)
	{
		std::cout << "closing socket in ListeningSocket class" << std::endl;
		closesocket(s);               
		setSock((INVALID_SOCKET));
	}
}

SOCKET HDE::ListeningSocket::acceptCon(sockaddr* address, int* addlen)
{
	SOCKET s = accept(getSock(), address, addlen);
	return s;
}
