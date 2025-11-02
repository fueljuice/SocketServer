#include "ClientSocketData.h"

HDE::ClientSocketData::ClientSocketData(int socket, sockaddr clientAddr, unsigned int length) 
		:
	clientSocket(socket),
	clientAddr(clientAddr),
	dataBuf(std::make_unique<char[]>(length)),
	lenData(length) {}


//bool HDE::ClientSocketData::operator==(const ClientSocketData& other)
//{
//	return this->clientSocket == other.clientSocket;
//}

HDE::ClientSocketData::~ClientSocketData()
{
    if (clientSocket != INVALID_SOCKET) {
        shutdown(clientSocket, SD_BOTH);
        closesocket(clientSocket);
    }
}




