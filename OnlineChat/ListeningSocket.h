#pragma once

#include "SimpleSocket.h"

namespace HDE
{

class ListeningSocket : public SimpleSocket
{

private:
	int backlog;
	int listening;

public:
	ListeningSocket(int domain, int service, int protocol, int port, u_long network_interaface, int backlog);

	void connectToNetwork(int sock, struct sockaddr_in address) override;

	int bindSocket(int sock, struct sockaddr_in address);

	void startLisetning();

	void stopLisetning();

	int acceptCon(sockaddr* address, int* addlen);


};

}