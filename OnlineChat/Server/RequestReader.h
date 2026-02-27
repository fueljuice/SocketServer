#pragma once

#include <WinSock2.h>
#include <optional>
#include "../Protocol/ParsedRequest.h"
#include "NetworkIO.h"
#include "SessionManager.h"

namespace sockets::server
{

class RequestReader
{
public:
    RequestReader(NetworkIO& net, SessionManager& sessions);

    std::optional<messaging::ParsedRequest> readNext(SOCKET sock);

private:
    NetworkIO& net;
    SessionManager& sessions;
};

}
