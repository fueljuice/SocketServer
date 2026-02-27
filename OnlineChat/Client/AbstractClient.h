#pragma once
#include <memory>
#include "../Protocol/ProtocolConstants.h"
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

	virtual void sendRequest(std::string msg, std::string recver, messaging::ActionType requestType) = 0;
	virtual std::string recieveResponse() = 0;

protected:
	// a socket that connects to a server. uncopyable to avoid dobule close
	std::unique_ptr<sockets::ConnectingSocket> conSocket;

};
}
