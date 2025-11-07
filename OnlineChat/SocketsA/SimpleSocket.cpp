
#include "SimpleSocket.h"


HDE::SimpleSocket::SimpleSocket(int domain, int service, int protocol, int port, u_long network_interaface)
{
	std::cout << "initiating socket..." << std::endl;
	initWSA();
	address.sin_family = domain;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(network_interaface);
	sock = socket(domain, service, protocol);
	std::cout << sock << std::endl;
	testConnection(sock);
	std::cout << "sucsessful socket init" << std::endl;
	
		
}

void HDE::SimpleSocket::testConnection(const SOCKET &sock) const
{
	if (sock < 0)
	{
		std::cout << sock << std::endl;
		perror("failed to zohar connect");
		exit(EXIT_FAILURE);

	}
}

void HDE::SimpleSocket::initWSA()
{
	if (WSAStartup(MAKEWORD(2, 2), &(this->wsa)) != 0)
	{
		printf("WSAStartup failed: %d\n", WSAGetLastError());
		perror("err WSA");
		closesocket(sock);
		WSACleanup();
	}
	std::cout << "WSA sucsess " << std::endl;
}

struct sockaddr_in HDE::SimpleSocket::getAddress() const
{
	return address;
}


SOCKET HDE::SimpleSocket::getSock() const
{
	return sock;
}



void HDE::SimpleSocket::setAddress(const struct sockaddr_in &address)
{
	this->address = address;
}

void HDE::SimpleSocket::setSock(const SOCKET &sock)
{
	this->sock = sock;
}

HDE::SimpleSocket::~SimpleSocket()
{
	std::cout << "destructing wsa and the socket" << std::endl;
	closesocket(sock);
	WSACleanup();
}