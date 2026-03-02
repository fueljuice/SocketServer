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
	gui(std::make_unique<GuiManager>()),
	net(std::make_unique<NetworkManager>(domain, service, protocol, port, network_interface)),
	respReader(std::make_unique<ResponseReader>(*net)),
	rqstSender(std::make_unique<RequestSender>(*net)),
	passiveListener(std::make_unique<PassiveListener>(*respReader, *net, *gui))
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

void Client::UserClient::sendToServer(std::string_view msg, messaging::RequestType action)
{
}

//void Client::UserClient::registerToServer(
//	std::string_view msg, 
//	std::string_view publicKey,
//	std::string_view privateKey, 
//	messaging::RequestType action)
//{
//	rqstSender->sendRequest(publicKey);
//	std::string keyAES = rsaDecrypt(respReader->readResponse(), privateKey)
//}
//


