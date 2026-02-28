#pragma once
#include <iostream>
#include <WinSock2.h>
#include <atomic>
#include <thread>
#include <chrono>
#include <optional>

#include "NetworkManager.h"
#include "ResponseReader.h"
#include "ClientExceptions.h"
namespace Client
{
struct IPassiveListener
{
    virtual void startPassiveListener() = 0;
    virtual void stopPassiveListener() = 0;
    virtual bool isListening() = 0;

};

class PassiveListener : public IPassiveListener
{
public:
    PassiveListener(ResponseReader& responseReader, INetworkManager& net);
    void startPassiveListener();
    void stopPassiveListener();
    bool isListening();

private:
    ResponseReader& reader;
    INetworkManager& net;
    std::atomic<bool> shouldListen;
    std::thread listenerThread;
    void passiveListenLoop();
    bool checkForMessages();
};
}

