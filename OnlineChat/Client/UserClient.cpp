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
	net(std::make_unique<NetworkManager>(domain, service, protocol, port, network_interface)),
	respReader(std::make_unique<ResponseReader>(*net)),
	rqstSender(std::make_unique<RequestSender>(*net)),
	passiveListener(std::make_unique<PassiveListener>(*respReader, *net))
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

void Client::UserClient::sendToServer(std::string_view msg, std::string_view rcver, messaging::ActionType action)
{
	rqstSender->sendRequest(msg, rcver, action);
}



