#include "UserClient.h"
#include "../Protocol/ProtocolConstants.h"
#include "ClientExceptions.h"


#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif

Client::UserClient::UserClient(int domain, int service, int protocol, int port, u_long network_interface)
	:
	rsa(std::make_unique<RSAWrapper>()),
	aes(std::make_unique<AESWrapper>()), 
	gui(std::make_unique<GuiManager>()),
	net(std::make_unique<NetworkManager>(domain, service, protocol, port, network_interface)),
	respReader(std::make_unique<ResponseReader>(*net)),
	handler(std::make_unique<ResponseHandler>(*aes, *rsa, *gui)),
	rqstSender(std::make_unique<RequestSender>(*net, *aes)),
	passiveListener(std::make_unique<PassiveListener>(*respReader, *net, *handler))
{
	DBG("UserClient ctor called. ");
}

Client::UserClient::~UserClient()
{
	stopClient();
}

void Client::UserClient::startClient()
{
	net->startNetwork();
	passiveListener->startPassiveListener();

}

void Client::UserClient::stopClient()
{
	net->closeNetwork();
	passiveListener->stopPassiveListener();
}

void Client::UserClient::getChat() const
{
	rqstSender->sendRequest("", "", messaging::RequestType::GET_CHAT);
}

void Client::UserClient::sendMessage(std::string_view msg) const
{
	rqstSender->sendRequest(msg, "", messaging::RequestType::SEND_MESSAGE);
}

void Client::UserClient::sendDirectMessage(std::string_view msg, std::string_view recver) const
{
	rqstSender->sendRequest(msg, recver, messaging::RequestType::DIRECT_MESSAGE);
}

void Client::UserClient::registerUser(std::string_view username) const
{
	rqstSender->sendRequest(username, "", messaging::RequestType::REGISTER);
}

bool Client::UserClient::sendPublicKey(int timeToWaitForResponse)
{
	if (!rsa->generateRSAKeyPair())
		return false;
	auto rsaStr = rsa->getPublicKey();
	if (!rsaStr)
		return false;
	rqstSender->sendRequest(rsaStr.value(), "", messaging::RequestType::SEND_RSA_PKEY);

	for (int i = 0; i < timeToWaitForResponse; ++i)
	{
		if (aes->hasKey())
			return true;

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return true;
}



