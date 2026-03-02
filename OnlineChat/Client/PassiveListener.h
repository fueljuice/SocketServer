#pragma once
#include <iostream>
#include <WinSock2.h>
#include <atomic>
#include <thread>
#include <chrono>
#include <optional>

#include "NetworkManager.h"
#include "ResponseReader.h"
#include "GuiManager.h"
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
    PassiveListener(IResponseReader& responseReader, INetworkManager& net, IGuiManager& gui);
    void startPassiveListener();
    void stopPassiveListener();
    bool isListening();

private:
    IResponseReader& reader;
    INetworkManager& net;
    IGuiManager& gui;
    std::atomic<bool> shouldListen;
    std::thread listenerThread;
    void passiveListenLoop();
    bool checkForMessages();
};
}

