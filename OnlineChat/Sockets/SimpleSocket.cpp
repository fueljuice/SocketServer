#include "SimpleSocket.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X
#else
#define DBG(X)
#endif // PR_DEBUG


// init the socket with the settings provided to the constructor
sockets::SimpleSocket::SimpleSocket(int domain, int service, int protocol, int port, u_long network_interaface)
{
	DBG("initiating socket...");
	// init WSA
	initWSA();

	// port domain and interface
	address.sin_family = domain;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = network_interaface;

	// gets the socket file descriptor from the os
	sock = socket(domain, service, protocol);
	DBG(sock);

	testConnection(sock);
	DBG("sucsessful socket init");


}
// exits if failed to make a valid socket
void sockets::SimpleSocket::testConnection(const SOCKET& sock) const
{
	if (sock < 0)
	{
		DBG(sock);
		perror("failed to zohar connect");
		exit(EXIT_FAILURE);

	}
}


// simply initWSA
void sockets::SimpleSocket::initWSA()
{
	if (WSAStartup(MAKEWORD(2, 2), &(this->wsa)) != 0)
	{
		perror("err WSA");
		closesocket(sock);
		WSACleanup();
	}
	DBG("WSA sucsess ");
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
void sockets::SimpleSocket::setAddress(const struct sockaddr_in& address)
{
	this->address = address;
}

void sockets::SimpleSocket::setSock(const SOCKET& sock)
{
	this->sock = sock;
}

// calls wsa cleanup and closes the socket
sockets::SimpleSocket::~SimpleSocket()
{
	DBG("destructing wsa and the socket");
	closesocket(sock);
	WSACleanup();
}