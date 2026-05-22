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

void Client::RequestSender::sendRequest(
    std::string_view msg,
    std::string_view recver,
    messaging::RequestType requestType)
{
    auto sendPayload = [&](std::string_view body)
        {
            const std::string payload =
                messaging::ClientProtocol::constructRequest(body, recver, requestType);

            DBG("data to send: " << payload);

            if (!net.sendAll(payload))
                throw ConnectionException("could not send request to server");

            DBG("sent request to server");
        };
    // direct message must have a recver
    if (requestType == messaging::RequestType::DIRECT_MESSAGE && recver.empty())
        throw ClientException("recver cannot be empty for direct message");

    // do not need to encrypt the rsa publick key
    if (requestType == messaging::RequestType::SEND_RSA_PKEY)
    {
        if (aes.hasKey())
            throw ClientException("AES key already established, cannot send RSA public key again");
        sendPayload(msg);
        return;
    }

	// must have a aes key for non RSA key requests
    if (!aes.hasKey())
        throw ClientException("AES key is not established yet");
    
    // dony encrypt if theres no body to the message
    if (msg.empty())
    {
        sendPayload("");
        return;
    }

    auto encryptedMsg = aes.encrypt(msg);
    if (!encryptedMsg)
        throw ClientException("AES encryption failed");

    sendPayload(encryptedMsg.value());
}

