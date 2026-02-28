#pragma once

#include <WinSock2.h>
#include <memory>

#include "../Protocol/ParsedRequest.h"
#include "NetworkIO.h"
#include "SessionManager.h"
#include "UserRegistry.h"
#include "RequestHandler.h"
#include "RequestReader.h"
#include "../Protocol/ServerProtocol.h"
#include "../Protocol/ProtocolConstants.h"

namespace sockets::server
{

class ClientConnectionWorker
{
public:
    ClientConnectionWorker(
        INetworkIO& net,
        ISessionManager& sessions,
        IUserRegistry& registry,
        RequestHandler& handler);

    void run(SOCKET sock);

private:
    INetworkIO& net;
    ISessionManager& sessions;
    IUserRegistry& registry;
    RequestHandler& handler;
    RequestReader reader;

    void removeDeadClient(SOCKET sock);
    void respond(SOCKET sock, messaging::ParsedRequest& parsed);
};
}
