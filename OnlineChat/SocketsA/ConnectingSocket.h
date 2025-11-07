#pragma once

#include "SimpleSocket.h"

namespace HDE
{


class ConnectingSocket : public SimpleSocket
{

public:
	ConnectingSocket(int domain, int service, int protocol, int port, u_long network_interface);

	void connectToNetwork(SOCKET sock, struct sockaddr_in address) override;

};

	 
}