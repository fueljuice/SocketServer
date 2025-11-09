#include "ClientSocketData.h"

HDE::ClientSocketData::ClientSocketData(SOCKET socket, sockaddr addr, unsigned int length)
	:
	clientSocket(socket),
	dataBuf(std::make_shared<char[]>(length)),
	lenData(length),
	clientAddr(addr)
	{}

// move constructor.
// the reason it is defined manually is to avoid closing the socket  on move
// since the destructor closes the socket.
// 

//HDE::ClientSocketData::ClientSocketData(ClientSocketData&& other) noexcept
//    : clientSocket(other.clientSocket),
//    clientAddr(other.clientAddr),
//    dataBuf(std::move(other.dataBuf)),
//    lenData(other.lenData)
//{
//
//    // this line ensures the dctr wont close the socket
//    other.clientSocket = INVALID_SOCKET; 
//
//    other.lenData = 0;
//    std::memset(&other.clientAddr, 0, sizeof(other.clientAddr));
//}


// destructor
// closing the socket if the object is descturcted to avoid leaking sockets
//HDE::ClientSocketData::~ClientSocketData()
//{
//	std::cout << "clientsocketdata destructed" << std::endl;
//    if (clientSocket != INVALID_SOCKET) {
//        shutdown(clientSocket, SD_BOTH);
//        closesocket(clientSocket);
//    }
//}




 
