#include "RequestReader.h"
#include "../Protocol/ServerProtocol.h"
#include "../Protocol/ProtocolConstants.h"
#include <iostream>

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

sockets::server::RequestReader::RequestReader(NetworkIO& net, SessionManager& sessions)
    : net(net), sessions(sessions) {}

std::optional<messaging::ParsedRequest> sockets::server::RequestReader::readNext(SOCKET sock)
{
    // recving data
    std::string header = net.recvAll(sock, messaging::REQUEST_HEADER_SIZE);
    if (header.size() != messaging::REQUEST_HEADER_SIZE)
    {
        DBG("recv failed");
        return std::nullopt;
    }
    
    // update metadata
    sessions.setClientHeader(sock, header);

    // parses the header
    auto parsedRqst = messaging::ServerProtocol::parseHeader(header.data(), header.size());
    if (!parsedRqst)
    {
        DBG("HEADER ERROR");
        return std::nullopt;
    }

    // skip reading if there is no data
    if (parsedRqst->dataSize == 0)
    {
        return parsedRqst;
    }
    
    // recving the body of the request
    DBG("recving data from client");
    std::string body = net.recvAll(sock, parsedRqst->dataSize);

    // sucsessful read. storing data and responding.
    DBG("done rcev data");
    if (body.size() != parsedRqst->dataSize)
    {
        DBG("recv failed");
        return std::nullopt;
    }
    
    sessions.setClientData(sock, body);
    return parsedRqst;
}
