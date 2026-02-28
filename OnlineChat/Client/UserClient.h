#pragma once

#include <string>
#include <iostream>
#include <stdio.h>
#include <memory>
#include <string>
#include <string_view>

#include "../Protocol/ClientProtocol.h"
#include "ClientExceptions.h"
#include "NetworkManager.h"
#include "ResponseReader.h"
#include "RequestSender.h"
#include "PassiveListener.h"
#include "../Protocol/ProtocolConstants.h"

namespace Client
{
struct IClient
{
    virtual ~IClient() = default;
    
    // start / close
    virtual void startClient() = 0;
    virtual void stopClient() = 0;

    virtual void sendToServer(std::string_view msg, std::string_view rcver, messaging::ActionType action) = 0;
};
class UserClient : public IClient
{
public:

    UserClient(int domain, int service, int protocol, int port, u_long network_interface);
    ~UserClient() override;
    void startClient() override;
    void stopClient() override;

    void sendToServer(std::string_view msg, std::string_view rcver, messaging::ActionType action) override;

private:
    std::unique_ptr<INetworkManager> net;
    std::unique_ptr<ResponseReader> respReader;
    std::unique_ptr<RequestSender> rqstSender;
    std::unique_ptr<IPassiveListener> passiveListener;
};
}