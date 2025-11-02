#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <memory>
namespace HDE
{

	struct ClientSocketData
	{
		int clientSocket;
		struct sockaddr clientAddr;
		std::unique_ptr<char[]> dataBuf;
		unsigned int lenData;

		ClientSocketData(int socket, struct sockaddr clientAddr, unsigned int length);

		// no copy because unique ptr
		ClientSocketData(const ClientSocketData&) = delete;
		ClientSocketData& operator=(const ClientSocketData&) = delete;

		// == opreator
		bool operator==(const ClientSocketData& other);


		~ClientSocketData();

		
	};


}