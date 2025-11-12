#include "ClientSocketData.h"

sockets::server::data::ClientSocketData::ClientSocketData(SOCKET socket, sockaddr addr, unsigned int length)
	:
	clientSocket(socket),
	dataBuf(std::make_shared<char[]>(length)),
	lenData(length),
	clientAddr(addr)
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




 
