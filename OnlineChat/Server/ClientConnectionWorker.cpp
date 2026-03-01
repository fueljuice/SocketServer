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
    IRequestHandler& handler)
    : net(net),
    sessions(sessions),
    registry(registry),
    handler(handler),
    reader(std::make_unique<RequestReader>(net, sessions)) {}

void sockets::server::ClientConnectionWorker::run(SOCKET sock)
{
    while (true)
    {
        // read from client
        auto req = reader->readNext(sock);
        if (!req)
        {
            removeDeadClient(sock);
            return;
        }
        // respond if hes avaiable
        handler.handleRequest(sock, req.value());
    }
}



void sockets::server::ClientConnectionWorker::removeDeadClient(SOCKET sock)
{
    // remove session and from registry
    sessions.removeClient(sock);
    registry.eraseClient(sock);
}

