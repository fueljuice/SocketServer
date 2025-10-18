
#include "SimpleSocket.h"


HDE::SimpleSocket::SimpleSocket(int domain, int service, int protocol, int port, u_long network_interaface)
{
	testWSA();
	address.sin_family = domain;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(network_interaface);
	sock = socket(domain, service, protocol);
	std::cout << sock << std::endl;
	testConnection(sock);
	
		
}

void HDE::SimpleSocket::testConnection(int sock)
{
	if (sock < 0)
	{
		std::cout << sock << std::endl;
		perror("failed to zoharconnect");
		exit(EXIT_FAILURE);

	}
}

void HDE::SimpleSocket::testWSA()
{
	if (WSAStartup(MAKEWORD(2, 2), &(this->wsa)) != 0)
	{
		printf("WSAStartup failed: %d\n", WSAGetLastError());
		perror("err WSA");
	}
}

struct sockaddr_in HDE::SimpleSocket::getAdress()
{
	return address;
}


int HDE::SimpleSocket::getSock()
{
	return sock;
}



void HDE::SimpleSocket::setAdress(struct sockaddr_in address)
{
	this->address = address;
}

void HDE::SimpleSocket::setSock(int sock)
{
	this->sock = sock;
}