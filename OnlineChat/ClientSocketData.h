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
		struct Handle
		{
			SOCKET socketHandle = INVALID_SOCKET;
		};

		SOCKET clientSocket;
		struct sockaddr clientAddr;
		std::unique_ptr<char[]> dataBuf;
		std::shared_ptr<Handle> h;
		unsigned int lenData;
		explicit ClientSocketData(SOCKET socket, sockaddr clientAddr, unsigned int length);

		// deleteing copy constructor to avoid copying the unique ptr
		ClientSocketData(const ClientSocketData&) = delete;
		ClientSocketData& operator=(const ClientSocketData&) = delete;


		// enabling std::move
		ClientSocketData(ClientSocketData&&) noexcept;
		ClientSocketData& operator=(ClientSocketData&&) noexcept = default;
		// == opreator
		//bool operator==(const ClientSocketData& other);

		


		~ClientSocketData();

		
	};


}