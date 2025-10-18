
#include "SimpleSocket.h"


HDE::SimpleSocket::SimpleSocket(int domain, int service, int protocol, int port, u_long network_interaface)
{
	initWSA();
	address.sin_family = domain;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(network_interaface);
	sock = socket(domain, service, protocol);
	std::cout << sock << std::endl;
	testConnection(sock);
	
		
}

void HDE::SimpleSocket::testConnection(const int &sock) const
{
	if (sock < 0)
	{
		std::cout << sock << std::endl;
		closesocket(sock);
		WSACleanup();
		perror("failed to zoharconnect");
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
}

struct sockaddr_in HDE::SimpleSocket::getAdress() const
{
	return address;
}


int HDE::SimpleSocket::getSock() const
{
	return sock;
}



void HDE::SimpleSocket::setAdress(const struct sockaddr_in &address)
{
	this->address = address;
}

void HDE::SimpleSocket::setSock(const int &sock)
{
	this->sock = sock;
}

HDE::SimpleSocket::~SimpleSocket()
{
	std::cout << "cleanup" << std::endl;
	closesocket(sock);
	WSACleanup();
}