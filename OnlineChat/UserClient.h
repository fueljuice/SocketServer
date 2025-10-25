#include "ClientInterface.h"


class UserClient : public ClientInterface
{
private:
    // Add this line to declare serverSock as a member of UserClient
   // HDE::ConnectingSocket serverSock;

public:
    UserClient(int domain, int service, int protocol, int port, u_long network_interface);

    void sendPacket();

    void recievePacket();
};