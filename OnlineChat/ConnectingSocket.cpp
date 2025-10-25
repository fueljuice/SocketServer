
#include "ConnectingSocket.h"

HDE::ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long network_interface)
	: SimpleSocket(domain, service, protocol, port, network_interface)
{
	connectToNetwork(getSock(), getAddress());
	testConnection(getSock());
};


void HDE::ConnectingSocket::connectToNetwork(int sock, struct sockaddr_in address)
{
	connect(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address));

}