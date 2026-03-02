#include "ResponseReader.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif

using optPrsdResp = std::optional<messaging::ParsedResponse>;
using optStr = std::optional<std::string>;
using StrAndCodePair = std::optional<std::pair<std::string, messaging::ResponseCode>>;

Client::ResponseReader::ResponseReader(INetworkManager& net_p)
    : net(net_p)
{
}



StrAndCodePair Client::ResponseReader::readResponse()
{
    // recving from server the header for the length
    DBG("recving header");
    optStr header = net.recvAll(messaging::RESPONSE_HEADER_SIZE);
    if (!header)
        return std::nullopt;
    DBG("RAWHEADER:" << header.value());
    // extracting info from header
    optPrsdResp prsdRqst = messaging::ClientProtocol::parseHeader(header.value(), header->size());
    if (!prsdRqst)
        return std::nullopt;
    DBG("data size from header: " << prsdRqst->dataSize);

    // dont recv data if it isnt expcted.
    if (prsdRqst->dataSize == 0)
        return std::pair("", prsdRqst->responseCode);

    // recving data
    DBG("recving data from server...");
    optStr dataResp = net.recvAll(prsdRqst->dataSize);
    if (!dataResp)
        return std::nullopt;

    // extracting data
    optPrsdResp refindPrsdRqst = messaging::ClientProtocol::parseData(std::move(*prsdRqst), *dataResp);
    if (!refindPrsdRqst)
        return std::nullopt;

    return std::pair(refindPrsdRqst->dataBuffer, refindPrsdRqst->responseCode);
}
