
#include "BindingSocket.h"

HDE::BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long network_interface)
	: SimpleSocket(domain, service, protocol, port, network_interface) 
{
	connectToNetwork(getSock(), getAdress());
	testConnection(getSock());
};


int HDE::BindingSocket::connectToNetwork(int sock, struct sockaddr_in address)
{
	return bind(sock, (struct sockaddr*)&address, sizeof(address));

}