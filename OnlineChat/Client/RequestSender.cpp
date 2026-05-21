#include "RequestSender.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif


Client::RequestSender::RequestSender(INetworkManager& net_p, IAESWrapper& aes_p)
    :
    net(net_p),
    aes(aes_p)
{
}

void Client::RequestSender::sendRequest(std::string_view msg, std::string_view recver, messaging::RequestType requestType)
{
	// if key was initialized, encrypt the message before sending
    if(aes.hasKey())
    {
        auto decryptedMsg = aes.encrypt(msg);
        if (!decryptedMsg)
            throw ClientException("encryption failed");

        msg = std::move(decryptedMsg.value());
    }
    // construct request
    std::string payload = buildRequest(msg, recver, requestType);

    // send it
    DBG("data to send: " << payload);
    if (!net.sendAll(payload))
        throw ConnectionException("could not send to client");
    DBG("sent to server:");
}

std::string Client::RequestSender::buildRequest(std::string_view msg, std::string_view recver, messaging::RequestType requestType)
{
    // construct request
    const size_t msgLength = msg.size() + recver.size();
    std::string payload = messaging::ClientProtocol::constructRequest(msg, recver, requestType);
    
    DBG("data to send: " << payload);
    return payload;
}
