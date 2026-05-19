#pragma once

#include <string>
#include <iostream>
#include <stdio.h>
#include <memory>
#include <string>
#include <string_view>

#include "ResponseHandler.h"
#include "NetworkManager.h"
#include "ResponseReader.h"
#include "RequestSender.h"
#include "PassiveListener.h"
#include "GuiManager.h"
#include "../Security/AESWrapper.h"
#include "../Security/RSAWrapper.h"

#include "../Protocol/ClientProtocol.h"
#include "../Protocol/ProtocolConstants.h"

namespace Client
{
struct IClient
{
    virtual ~IClient() = default;
    
    // start / close
    virtual void startClient() = 0;
    virtual void stopClient() = 0;

    virtual void sendToServer(std::string_view msg, std::string_view rcver, messaging::RequestType action) = 0;
    virtual void sendToServer(std::string_view msg, messaging::RequestType action) = 0;
    virtual bool sendPublicKey() = 0;

};
class UserClient : public IClient
{
public:

    UserClient(int domain, int service, int protocol, int port, u_long network_interface);
    ~UserClient() override;
    void startClient() override;
    void stopClient() override;

    void sendToServer(std::string_view msg, std::string_view rcver, messaging::RequestType rqstType) override;
    void sendToServer(std::string_view msg, messaging::RequestType rqstType) override;
    bool sendPublicKey() override;

private:
    std::unique_ptr<RSAWrapper> rsa;
    std::unique_ptr<AESWrapper> aes;
    std::unique_ptr<IGuiManager> gui;
    std::unique_ptr<INetworkManager> net;
    std::unique_ptr<IResponseReader> respReader;
    std::unique_ptr<IResponseHandler> handler;
    std::unique_ptr<IRequestSender> rqstSender;
    std::unique_ptr<IPassiveListener> passiveListener;

};
}