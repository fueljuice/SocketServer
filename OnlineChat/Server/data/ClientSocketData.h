#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <memory>
#include <iostream>
namespace sockets::server::data
{

	struct ClientSocketData
	{
		// ClientSocketData is made in order to keep all the client's data in one place,
		// including its socket, sockaddr struct length of the buffer and the buffer itself
		SOCKET clientSocket;
		struct sockaddr clientAddr;
		std::shared_ptr<char[]> dataBuf;
		unsigned int lenData;


		explicit ClientSocketData(SOCKET socket, sockaddr clientAddr, unsigned int length);

		// enabling copy constructor
		ClientSocketData(const ClientSocketData&) = default;
		ClientSocketData& operator=(const ClientSocketData&) = default;


		// enabling std::move
		ClientSocketData(ClientSocketData&&) noexcept = default;
		ClientSocketData& operator=(ClientSocketData&&) noexcept = default;


		


		~ClientSocketData();

		
	};


}