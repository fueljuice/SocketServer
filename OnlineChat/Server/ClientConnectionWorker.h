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
struct IClientConnectionWorker
{
    virtual void run(SOCKET sock) = 0;
};
class ClientConnectionWorker : public IClientConnectionWorker
{
public:
    ClientConnectionWorker(
        INetworkIO& net,
        ISessionManager& sessions,
        IUserRegistry& registry,
        IRequestHandler& handler);

    void run(SOCKET sock) override;

private:
    INetworkIO& net;
    ISessionManager& sessions;
    IUserRegistry& registry;
    IRequestHandler& handler;
    std::unique_ptr<RequestReader> reader;

    void removeDeadClient(SOCKET sock);
};
}
