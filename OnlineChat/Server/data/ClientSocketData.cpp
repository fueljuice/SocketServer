#include "ClientSocketData.h"
#ifdef PR_DEBUG
#define DBG(X) std::cout << X
#else
#define DBG(X)
#endif // PR_DEBUG
sockets::server::data::ClientSocketData::ClientSocketData(SOCKET socket, sockaddr addr)
	:
	clientSocket(socket),
	clientAddr(addr)
	{}


 //destructor
 //closing the socket if the object is descturcted to avoid leaking sockets
sockets::server::data::ClientSocketData::~ClientSocketData()
{
	
	DBG("clientsocketdata destructed");
	std::cout << "User left the server" << std::endl;
    if (clientSocket != INVALID_SOCKET) 
	{
        shutdown(clientSocket, SD_BOTH);
        closesocket(clientSocket);
    }
}




 
