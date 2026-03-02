#pragma once
#include <string>
#include "../Protocol/ClientProtocol.h"
#include "NetworkManager.h"
#include "./ClientExceptions.h"
namespace Client
{
struct IRequestSender
{
    virtual void sendRequest(std::string_view msg, std::string_view recver, messaging::RequestType requestType) = 0;
};
class RequestSender : public IRequestSender
{
public:
    RequestSender(INetworkManager& net);
    void sendRequest(std::string_view msg, std::string_view recver, messaging::RequestType requestType);
    
private:
    INetworkManager& net;
    std::string buildRequest(std::string_view msg, std::string_view recver, messaging::RequestType requestType);
};
}
