#include "ClientConnectionWorker.h"


#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

sockets::server::ClientConnectionWorker::ClientConnectionWorker(
    INetworkIO& net,
    ISessionManager& sessions,
    IUserRegistry& registry,
    RequestHandler& handler)
    : net(net),
    sessions(sessions),
    registry(registry),
    handler(handler),
    reader(net, sessions) {}

void sockets::server::ClientConnectionWorker::run(SOCKET sock)
{
    while (true)
    {
        auto req = reader.readNext(sock);
        if (!req)
        {
            removeDeadClient(sock);
            return;
        }
        respond(sock, req.value());
    }
}

void sockets::server::ClientConnectionWorker::removeDeadClient(SOCKET sock)
{
    // remove session and from registry
    sessions.removeClient(sock);
    registry.eraseClient(sock);
}

void sockets::server::ClientConnectionWorker::respond(SOCKET sock, messaging::ParsedRequest& oldParsedRqst)
{
	std::string rawData = sessions.getClientData(sock).data();
    auto refinedPr = 
        messaging::ServerProtocol::parseData(std::move(oldParsedRqst),
            rawData.data());
    if(!refinedPr)
    {
        DBG("DATA ERROR");
        return;
	}
    handler.handleRequest(sock, refinedPr.value());
   
} 
