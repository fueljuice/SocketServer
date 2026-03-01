#pragma once

#include <WinSock2.h>
#include <optional>
#include <utility>

#include "../Protocol/ParsedRequest.h"
#include "NetworkIO.h"
#include "SessionManager.h"

namespace sockets::server
{

class RequestReader
{
public:
    RequestReader(INetworkIO& net, ISessionManager& sessions);
    std::optional<messaging::ParsedRequest> readNext(SOCKET sock);

private:
    INetworkIO& net;
    ISessionManager& sessions;
};

}
