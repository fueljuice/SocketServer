#pragma once

#include <string>
#include <iostream>
#include <stdio.h>

#include "AbstractClient.h"


namespace Client
{
    enum action
    {
        GECHAT = 1,
        SENDMESSAGE = 2

    };

    class UserClient : public AbstractClient
    {

    public:

        UserClient(int domain, int service, int protocol, int port, u_long network_interface);
        // sends a packet to the server
        void sendPacket(const char* msg, u_int requestType) override;
        // recieves a packet from the server
        std::string recievePacket() override;


    private:
        // uses SENDMESSAGE request type
        void sendMessage(u_int msgLength, const char* msg, u_int requestType);
        // uses GETCHAT request type
        void getChat(u_int requestType);

        bool sendAll(SOCKET s, char* buf, u_int len);
    };
}