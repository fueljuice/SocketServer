#pragma once

#include "BindingSocket.h"

namespace HDE
{

class ListeningSocket : public BindingSocket
{

private:
	int backlog;
	int listening;

public:
	ListeningSocket(int domain, int service, int protocol, int port, u_long network_interaface, int backlog);

	void startLisetning();

};

}