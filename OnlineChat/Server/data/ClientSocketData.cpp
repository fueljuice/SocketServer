#include "ClientSocketData.h"

void sockets::server::data::ClientSocketData::initData(unsigned int length)
{
	dataBuf = std::make_shared<char[]>(length);
}

sockets::server::data::ClientSocketData::ClientSocketData(SOCKET socket, sockaddr addr)
	:
	clientSocket(socket),
	dataBuf(nullptr),
	lenData(-1),
	clientAddr(addr),
	isRegistered(false)
	{}


 //destructor
 //closing the socket if the object is descturcted to avoid leaking sockets
sockets::server::data::ClientSocketData::~ClientSocketData()
{
	std::cout << "clientsocketdata destructed" << std::endl;
    if (clientSocket != INVALID_SOCKET) 
	{
        shutdown(clientSocket, SD_BOTH);
        closesocket(clientSocket);
    }
}




 
