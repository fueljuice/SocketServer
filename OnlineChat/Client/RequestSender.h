#pragma once
#include <string>
#include "../Protocol/ClientProtocol.h"
#include "NetworkManager.h"
#include "./ClientExceptions.h"
namespace Client
{
class RequestSender
{
public:
    RequestSender(INetworkManager& net);
    void sendRequest(std::string_view msg, std::string_view recver, messaging::ActionType requestType);
    
private:
    INetworkManager& net;
    std::string buildRequest(std::string_view msg, std::string_view recver, messaging::ActionType requestType);
};
}
