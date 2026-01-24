#pragma once

#include "SimpleSocket.h"

namespace sockets
{

class ListeningSocket : public SimpleSocket
{
// inherits from the simple socket class.
// a listening socket that is made to be used as the server's socket
public:
	ListeningSocket(int domain, int service, int protocol, int port, u_long network_interaface, int backlog);
	// starts a listen
	void startLisetning();
	// stops te listen
	void stopLisetning();
	// accepts a connection
	SOCKET acceptCon(sockaddr* address, int* addlen);



private:
	// implements the abstract function
	void connectToNetwork(SOCKET sock, struct sockaddr_in address) override;

	SOCKET bindSocket(SOCKET sock, struct sockaddr_in address);

	int backlog;
	int listening;
};
		


}