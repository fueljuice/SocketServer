#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <memory>
#include <iostream>
namespace sockets::server::data
{
	// ClientSocketData is made in order to keep all the client's data in one place,
// including its socket, sockaddr struct length of the buffer and the buffer itself
struct ClientSocketData
{
	// client's socket
	SOCKET clientSocket;
	// client's sockaddr struct
	struct sockaddr clientAddr;
	// recved data buffer
	std::shared_ptr<char[]> dataBuf; // CHANGE
	// the recved data buffer's length
	unsigned int lenData;

	void initData(unsigned int length);


	explicit ClientSocketData(SOCKET socket, sockaddr clientAddr);

	// enabling copy constructor
	ClientSocketData(const ClientSocketData&) = default;
	ClientSocketData& operator=(const ClientSocketData&) = default;


	// enabling std::move
	ClientSocketData(ClientSocketData&&) noexcept = default;
	ClientSocketData& operator=(ClientSocketData&&) noexcept = default;

	
		


	~ClientSocketData();

		
};
}