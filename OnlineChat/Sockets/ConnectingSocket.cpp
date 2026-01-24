#include "ConnectingSocket.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X)
#else
#define DBG(X)
#endif // PR_DEBUG

sockets::ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long network_interface)
	: SimpleSocket(domain, service, protocol, port, network_interface)
{
	DBG("con ctor");
}

void sockets::ConnectingSocket::startConnect()
{
	DBG("starting connection...");

	connectToNetwork(getSock(), getAddress());
	testConnection(getSock());
	DBG("connected to socket " << getSock() << " sucsessfuly");
}

void sockets::ConnectingSocket::stopConnection()
{
	SOCKET s = getSock();
	if (s != INVALID_SOCKET)
	{
		std::cout << "closing socket in ConnectingSocket class");
		shutdown(s, SD_BOTH);
		closesocket(s);
		setSock((INVALID_SOCKET));
	}
}


void sockets::ConnectingSocket::connectToNetwork(SOCKET sock, struct sockaddr_in address)
{
	DBG("starting connection...");


	DBG("Connecting to "
		<< int(address.sin_addr.S_un.S_un_b.s_b1) << "."
		<< int(address.sin_addr.S_un.S_un_b.s_b2) << "."
		<< int(address.sin_addr.S_un.S_un_b.s_b3) << "."
		<< int(address.sin_addr.S_un.S_un_b.s_b4) << ":"
		<< ntohs(address.sin_port));

	connect(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address));

}