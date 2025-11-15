#include "ListeningSocket.h"


sockets::ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long network_interface, int bklog)
	: 
	SimpleSocket(domain, service, protocol, port, htonl(network_interface)),
	backlog(bklog)
{
	connectToNetwork(getSock(), getAddress());
}



// init the socket server
void sockets::ListeningSocket::connectToNetwork(SOCKET sock, sockaddr_in address)
{
	bindSocket(sock, address);
	//testing binding
	testConnection(getSock());
	std::cout << "binded sucsessfuly to port" << getAddress().sin_port << std::endl;


}


// binds socket to port
SOCKET sockets::ListeningSocket::bindSocket(SOCKET sock, sockaddr_in address)
{
	return bind(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address));
}


// starts listening on binded port
void sockets::ListeningSocket::startLisetning()
{
	listening = listen(getSock(), backlog);
	testConnection(listening);
	std::cout << "started lisetning sucsessfuly on port" << getAddress().sin_port << std::endl;

}


// stops listening
void sockets::ListeningSocket::stopLisetning()
{
	SOCKET s = getSock();
	if (s != INVALID_SOCKET)
	{
		std::cout << "closing socket in ListeningSocket class" << std::endl;
		shutdown(s, SD_BOTH);
		closesocket(s);               
		setSock((INVALID_SOCKET));
	}
}


// accepts a connection and returns its socket
SOCKET sockets::ListeningSocket::acceptCon(sockaddr* address, int* addlen)
{
	SOCKET s = accept(getSock(), address, addlen);
	return s;
}
