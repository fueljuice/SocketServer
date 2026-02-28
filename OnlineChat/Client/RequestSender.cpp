#include "RequestSender.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif

Client::RequestSender::RequestSender(INetworkManager& net_p)
    : net(net_p)
{
}

void Client::RequestSender::sendRequest(std::string_view msg, std::string_view recver, messaging::ActionType requestType)
{
    // construct request
    std::string payload = buildRequest(msg, recver, requestType);

    // send it
    DBG("data to send: " << payload);
    if (!net.sendAll(payload))
        throw ConnectionException("could not send to client");
    DBG("sent to server:");
}

std::string Client::RequestSender::buildRequest(std::string_view msg, std::string_view recver, messaging::ActionType requestType)
{
    // construct request
    const size_t msgLength = msg.size() + recver.size();
    std::string payload = messaging::ClientProtocol::constructRequest(msg, recver, requestType);
    
    DBG("data to send: " << payload);
    return payload;
}
