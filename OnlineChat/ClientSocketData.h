#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <memory>
#include <iostream>
namespace HDE
{

	struct ClientSocketData
	{

		SOCKET clientSocket;
		struct sockaddr clientAddr;
		std::shared_ptr<char[]> dataBuf;
		unsigned int lenData;

		explicit ClientSocketData(SOCKET socket, sockaddr clientAddr, unsigned int length);

		ClientSocketData(const ClientSocketData&) = default;
		ClientSocketData& operator=(const ClientSocketData&) = default;


		// enabling std::move
		ClientSocketData(ClientSocketData&&) noexcept = default;
		ClientSocketData& operator=(ClientSocketData&&) noexcept = default;
		// == opreator
		//bool operator==(const ClientSocketData& other);

		


		~ClientSocketData() = default;

		
	};


}