#pragma once


#include <memory>

#include "../Sockets/ConnectingSocket.h"
namespace Client
{
class AbstractClient
{
public:
	AbstractClient(int domain, int service, int protocol, int port, u_long network_interface);

	// delete copy constructors and asignment operators because of the a unique ptr.
	virtual ~AbstractClient();
	AbstractClient(const AbstractClient&) = delete;
	AbstractClient& operator=(const AbstractClient&) = delete;

	virtual void sendPacket(const char* msg, u_int requestType) = 0;

	virtual std::string recievePacket() = 0;

protected:
	// a socket that connects to a server. uncopyable to avoid dobule close
	std::unique_ptr<sockets::ConnectingSocket> conSocket;

};
}
