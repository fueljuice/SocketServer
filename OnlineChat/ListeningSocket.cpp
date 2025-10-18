

#include "ListeningSocket.h"


HDE::ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long network_interaface, int backlog)
	: BindingSocket(domain, service, protocol, port, network_interaface)
{
	this->backlog = backlog;
	startLisetning();
	testConnection(listening);

}

void HDE::ListeningSocket::startLisetning()
{
	std::cout << "here" << std::endl;
	listening = listen(getSock(), backlog);
}
