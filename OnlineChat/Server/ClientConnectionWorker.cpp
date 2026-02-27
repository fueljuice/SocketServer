#include "ClientConnectionWorker.h"


#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

sockets::server::ClientConnectionWorker::ClientConnectionWorker(
    NetworkIO& net,
    SessionManager& sessions,
    UserRegistry& registry,
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
    auto refinedPr = 
        messaging::ServerProtocol::parseData(std::move(oldParsedRqst),
        sessions.getClientData(sock).data());
    handler.handleRequest(sock, refinedPr);
   
} 
