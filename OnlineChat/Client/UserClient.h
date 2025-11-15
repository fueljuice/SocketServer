#pragma once

#include <string>
#include <iostream>
#include <stdio.h>

#include "ClientInterface.h"


namespace Client
{
    enum action
    {
        GECHAT = 1,
        SENDMESSAGE = 2

    };

    class UserClient : public ClientInterface
    {

    public:

        UserClient(int domain, int service, int protocol, int port, u_long network_interface);

        void sendPacket(const char* msg, u_int requestType) override;

        std::string recievePacket() override;


    private:
        void sendMessage(u_int msgLength, const char* msg, u_int requestType);

        void getChat(u_int requestType);

        bool sendAll(SOCKET s, char* buf, u_int len);
    };
}