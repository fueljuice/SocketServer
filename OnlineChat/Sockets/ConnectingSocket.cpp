#include "ConnectingSocket.h"

sockets::ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long network_interface)
	: SimpleSocket(domain, service, protocol, port, network_interface)
{
	std::cout << "con ctor" << std::endl;
}

void sockets::ConnectingSocket::startConnect()
{
	std::cout << "starting connection..." << std::endl;

	connectToNetwork(getSock(), getAddress());
	testConnection(getSock());
	std::cout << "connected to socket " << getSock() << " sucsessfuly" << std::endl;
}

void sockets::ConnectingSocket::stopConnection()
{
	SOCKET s = getSock();
	if (s != INVALID_SOCKET)
	{
		std::cout << "closing socket in ConnectingSocket class" << std::endl;
		shutdown(s, SD_BOTH);
		closesocket(s);
		setSock((INVALID_SOCKET));
	}
}


void sockets::ConnectingSocket::connectToNetwork(SOCKET sock, struct sockaddr_in address)
{
	std::cout << "starting connection..." << std::endl;


	std::cout << "Connecting to "
		<< int(address.sin_addr.S_un.S_un_b.s_b1) << "."
		<< int(address.sin_addr.S_un.S_un_b.s_b2) << "."
		<< int(address.sin_addr.S_un.S_un_b.s_b3) << "."
		<< int(address.sin_addr.S_un.S_un_b.s_b4) << ":"
		<< ntohs(address.sin_port) << std::endl;

	connect(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address));

}