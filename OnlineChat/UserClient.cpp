#include "UserClient.h"

UserClient::UserClient(int domain, int service, int protocol, int port, u_long network_interface) 
    : serverSock(port)

    {}

void UserClient::sendPacket()
{
}

void UserClient::recievePacket()
{
}

