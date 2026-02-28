#include "RequestReader.h"
#include "../Protocol/ServerProtocol.h"
#include "../Protocol/ProtocolConstants.h"
#include <iostream>

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

sockets::server::RequestReader::RequestReader(INetworkIO& net, ISessionManager& sessions)
    : net(net), sessions(sessions) {}

std::optional<messaging::ParsedRequest> sockets::server::RequestReader::readNext(SOCKET sock)
{
    DBG("recving header");
    // recving data
    auto header = net.recvAll(sock, messaging::REQUEST_HEADER_SIZE);
    if (!header)
    {
        DBG("recv failed");
        return std::nullopt;
    }
    
    // update metadata
    sessions.setClientHeader(sock, header.value());

    // parses the header
    auto parsedRqst = messaging::ServerProtocol::parseHeader(*header, (*header).size());
    if (!parsedRqst)
        return std::nullopt;

    // skip reading if there is no data
    DBG("HEADER OK");
    if (parsedRqst->dataSize == 0)
        return parsedRqst;
    
    // recving the body of the request
    DBG("recving data from client");
    auto data = net.recvAll(sock, parsedRqst->dataSize);
    DBG("done rcev data");
    if (!data)
    {
        DBG("recv failed");
        return std::nullopt;
    }
    // sucsessful read, update metadata
    sessions.setClientData(sock, data.value());
    return parsedRqst;
}
