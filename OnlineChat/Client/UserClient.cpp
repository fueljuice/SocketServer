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

void Client::UserClient::sendToServer(std::string_view msg, std::string_view rcver, messaging::RequestType rqstType)
{
	rqstSender->sendRequest(msg, rcver, rqstType);
}

void Client::UserClient::sendToServer(std::string_view msg, messaging::RequestType rqstType)
{
	rqstSender->sendRequest(msg, "", rqstType);
}

bool Client::UserClient::sendPublicKey()
{
	if (!rsa->generateRSAKeyPair())
		return false;
	auto rsaStr = rsa->getPublicKey();
	if (!rsaStr)
		return false;
	rqstSender->sendRequest(rsaStr.value(), "", messaging::RequestType::SEND_RSA_PKEY);
	return true;
}



