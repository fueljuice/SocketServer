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
	
public:

	SimpleSocket(int domain, int service, int protocol, int port, u_long network_interaface);


	virtual int connectToNetwork(int sock, struct sockaddr_in address) = 0;
	
	// checks the socket file descriptor
	void testConnection(int item_to_test);

	// setup WSA and raise error if fail
	void initWSA();


	//getter
	struct sockaddr_in getAdress();
	int getSock();

	//setter
	void setAdress(struct sockaddr_in add);
	void setSock(int sock);
	
};



}