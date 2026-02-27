#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>


namespace sockets::server::data
{
	// ClientSocketData is made in order to keep all the client's data in one place,
// including its socket, sockaddr struct length of the buffer and the buffer itself
struct ClientSocketData
{

	~ClientSocketData();
	ClientSocketData(SOCKET socket, sockaddr clientAddr);

	SOCKET clientSocket;
	struct sockaddr clientAddr;
	std::string dataBuf; 
	std::string headerBuf;
};
}