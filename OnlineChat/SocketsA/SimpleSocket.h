
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
	public:

		SimpleSocket(int domain, int service, int protocol, int port, u_long network_interaface);


		virtual void connectToNetwork(SOCKET sock, struct sockaddr_in address) = 0;

		// checks the socket file descriptor
		void testConnection(const SOCKET& sock) const;

		//getter
		struct sockaddr_in getAddress() const;
		SOCKET getSock() const;

		//setter
		void setAddress(const struct sockaddr_in& add);
		void setSock(const SOCKET& sock);

		virtual ~SimpleSocket();


	private:
		WSADATA wsa;
		struct sockaddr_in address;
		SOCKET sock;



		// setup WSA and raise error if fail
		void initWSA();
	

	
	};



}