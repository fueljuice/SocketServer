
#include "SimpleSocket.h"

// init the socket with the settings provided to the constructor
sockets::SimpleSocket::SimpleSocket(int domain, int service, int protocol, int port, u_long network_interaface)
{
	std::cout << "initiating socket..." << std::endl;
	// init WSA
	initWSA();

	// port domain and interface
	address.sin_family = domain;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(network_interaface);

	// gets the socket file descriptor from the os
	sock = socket(domain, service, protocol);
	std::cout << sock << std::endl;

	testConnection(sock);
	std::cout << "sucsessful socket init" << std::endl;
	
		
}
// exits if failed to make a valid socket
void sockets::SimpleSocket::testConnection(const SOCKET &sock) const
{
	if (sock < 0)
	{
		std::cout << sock << std::endl;
		perror("failed to zohar connect");
		exit(EXIT_FAILURE);

	}
}


// simply initWSA
void sockets::SimpleSocket::initWSA()
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

// setters
struct sockaddr_in sockets::SimpleSocket::getAddress() const
{
	return address;
}


SOCKET sockets::SimpleSocket::getSock() const
{
	return sock;
}


// getters 
void sockets::SimpleSocket::setAddress(const struct sockaddr_in &address)
{
	this->address = address;
}

void sockets::SimpleSocket::setSock(const SOCKET &sock)
{
	this->sock = sock;
}

// calls wsa cleanup and closes the socket
sockets::SimpleSocket::~SimpleSocket()
{
	std::cout << "destructing wsa and the socket" << std::endl;
	closesocket(sock);
	WSACleanup();
}