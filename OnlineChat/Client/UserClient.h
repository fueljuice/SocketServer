#pragma once

#include <string>
#include <iostream>
#include <stdio.h>
#include <array>

#include "../Protocol/ClientProtocol.h"
#include "AbstractClient.h"


namespace Client
{
class UserClient : public AbstractClient
{

public:

    UserClient(int domain, int service, int protocol, int port, u_long network_interface);
    // sends a packet to the server
    void sendRequest(const char* msg, u_int requestType, const char* name) override;
    // recieves a packet from the server
    std::string recieveResponse() override;


private:
    // uses SENDMESSAGE request type
    void sendRequestInternal(u_int msgLength, const char* msg, u_int requestType, const char* userName);
    bool sendAll(SOCKET s, const char* buf, u_int len);
};
}