#pragma once
#include <string>
#include <utility>


#include "../Protocol/ClientProtocol.h"
#include "NetworkManager.h"
#include "./ClientExceptions.h"
#include "../Security/AESWrapper.h"
namespace Client
{
struct IRequestSender
{
    virtual void sendRequest(std::string_view msg, std::string_view recver, messaging::RequestType requestType) = 0;
};
class RequestSender : public IRequestSender
{
public:
    RequestSender(INetworkManager& net, IAESWrapper& aes);
    void sendRequest(std::string_view msg, std::string_view recver, messaging::RequestType requestType);
    
private:
    INetworkManager& net;
    IAESWrapper& aes;
    std::string buildRequest(std::string_view msg, std::string_view recver, messaging::RequestType requestType);
};
}
