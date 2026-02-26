#pragma once

#include <string>
#include <iostream>
#include <stdio.h>
#include <array>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>

#include "../Protocol/ClientProtocol.h"
#include "AbstractClient.h"
#include "ClientExceptions.h"


namespace Client
{
    class UserClient : public AbstractClient
    {

    public:

        UserClient(int domain, int service, int protocol, int port, u_long network_interface);
        ~UserClient();

        // sends a packet to the server
        void sendRequest(const char* msg, u_int requestType) override;
        // recieves a packet from the server
        std::string recieveResponse() override;

        // passive listening methods
        void startPassiveListener();
        void stopPassiveListener();

    private:
        // uses SENDMESSAGE request type
        void sendRequestInternal(u_int msgLength, const char* msg, u_int requestType);
        bool sendAll(SOCKET s, const char* buf, u_int len);

        // passive listening members
        void passiveListenLoop();
        bool checkForMessages();

        std::thread listenerThread;
        std::atomic<bool> shouldListen{ false };
    };
}