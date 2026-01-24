#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdio>
#include <iostream>
namespace sockets
{

class SimpleSocket
{

public:
	// the simplesocket class goal is to 
	// set up the socket basic low level settings: domain service protocol port and interface
	SimpleSocket(int domain, int service, int protocol, int port, u_long network_interaface);


	//getters
	struct sockaddr_in getAddress() const;
	SOCKET getSock() const;

	//setters
	void setAddress(const struct sockaddr_in& add);
	void setSock(const SOCKET& sock);

	// virtual so that every sucsessor will call this aswell
	virtual ~SimpleSocket();


protected:
	// should only be used in derived class initalizor functions to test the connection
	void testConnection(const SOCKET& sock) const;


private:
	// all its sucsessors must provide a way of connecting to the network
	virtual void connectToNetwork(SOCKET sock, struct sockaddr_in address) = 0;


	WSADATA wsa;
	struct sockaddr_in address;
	SOCKET sock;



	// setup WSA and raise error if fail
	void initWSA();



};
}