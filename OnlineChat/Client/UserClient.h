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

    virtual void getChat() const = 0;
    virtual void sendMessage(std::string_view msg) const = 0;
    virtual void sendDirectMessage(std::string_view msg, std::string_view recver) const = 0;
    virtual void registerUser(std::string_view username) const = 0;
    virtual bool sendPublicKey(int timeToWaitForResponse) = 0;

};
class UserClient : public IClient
{
public:

    UserClient(int domain, int service, int protocol, int port, u_long network_interface);
    ~UserClient() override;
    void startClient() override;
    void stopClient() override;

	void getChat() const override;
	void sendMessage(std::string_view msg) const override;
	void sendDirectMessage(std::string_view msg, std::string_view recver) const override;
	void registerUser(std::string_view username) const override;
    bool sendPublicKey(int timeToWaitForResponse) override;

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