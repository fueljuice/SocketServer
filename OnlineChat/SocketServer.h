#pragma once
#include "ListeningSocket.h"


namespace HDE
{

struct clientSocketData
{
	int clientSocket;
	struct sockaddr clientAddr;
	std::unique_ptr<char[]> dataBuf;
	unsigned int lenData;

	clientSocketData(int socket, struct sockaddr clientAddr, unsigned int length)
		: 
		clientSocket(socket), 
		clientAddr(clientAddr),
		dataBuf(std::make_unique<char[]>(length)),
		lenData(length) {}

	// Add move constructor
	clientSocketData(clientSocketData&& other) noexcept
		: clientSocket(other.clientSocket),
		clientAddr(other.clientAddr),
		dataBuf(std::move(other.dataBuf)),
		lenData(other.lenData)
	{
	}

	// Add move assignment operator
	clientSocketData& operator=(clientSocketData&& other) noexcept
	{
		if (this != &other)
		{
			clientSocket = other.clientSocket;
			clientAddr = other.clientAddr;
			dataBuf = std::move(other.dataBuf);
			lenData = other.lenData;
		}
		return *this;
	}

	// Delete copy constructor and copy assignment operator
	clientSocketData(const clientSocketData&) = delete;
	clientSocketData& operator=(const clientSocketData&) = delete;

	// Add equality operator for std::remove
	bool operator==(const clientSocketData& other) const
	{
		return clientSocket == other.clientSocket;
	}
};

class SocketServer
{

private:
	// accepts a socket and returns a new socket that was created by the accept function
	virtual clientSocketData acceptConnection() = 0;

	virtual void handleConnection() = 0;

	virtual void responder() = 0;

protected:
	std::unique_ptr<ListeningSocket> lstnSocket;


public:
	SocketServer(int domain, int service, int protocol, 
		int port, u_long network_interaface, int backlog);


	virtual void launch() = 0;

	// rule of three
	virtual ~SocketServer() = default;
	SocketServer(const SocketServer&) = delete;  // unique_ptr  noncopyable
	SocketServer& operator=(const SocketServer&) = delete;


};
}