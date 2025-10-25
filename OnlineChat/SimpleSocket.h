
#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
#include <iostream>
namespace HDE
{

class SimpleSocket
{
private:
	WSADATA wsa;
	struct sockaddr_in address;
	int sock;



	// setup WSA and raise error if fail
	void initWSA();
	
public:

	SimpleSocket(int domain, int service, int protocol, int port, u_long network_interaface);


	virtual void connectToNetwork(int sock, struct sockaddr_in address) = 0;
	
	// checks the socket file descriptor
	void testConnection(const int &sock) const;

	//getter
	struct sockaddr_in getAddress() const;
	int getSock() const;

	//setter
	void setAddress(const struct sockaddr_in &add);
	void setSock(const int& sock);

	virtual ~SimpleSocket();
	
};



}