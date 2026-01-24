#pragma once

#include "SimpleSocket.h"

namespace sockets
{
class ConnectingSocket : public SimpleSocket
{

public:
	ConnectingSocket(int domain, int service, int protocol, int port, u_long network_interface);
	// starts a connection
	void startConnect();
	// stops the connection
	void stopConnection();



private:
	// connecting to network
	void connectToNetwork(SOCKET sock, struct sockaddr_in address) override;
};


}