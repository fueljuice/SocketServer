#include "ConnectingSocket.h"

sockets::ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long network_interface)
	: SimpleSocket(domain, service, protocol, port, network_interface)
{
	connectToNetwork(getSock(), getAddress());
	testConnection(getSock());
}


void sockets::ConnectingSocket::connectToNetwork(SOCKET sock, struct sockaddr_in address)
{
	connect(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address));

}