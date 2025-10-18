#pragma once

#include "SimpleSocket.h"

namespace HDE
{
	class ConnectingSocket : SimpleSocket
	{

	public:
		ConnectingSocket(int domain, int service, int protocol, int port, u_long network_interface);

		int connectToNetwork(int sock, struct sockaddr_in address) override;

	};

	 
}