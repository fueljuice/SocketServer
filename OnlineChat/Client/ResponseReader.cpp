#include "ResponseReader.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif

using optPrsdResp = std::optional<messaging::ParsedResponse>;
using optStr = std::optional<std::string>;

Client::ResponseReader::ResponseReader(INetworkManager& net_p)
    : net(net_p)
{}



optStr Client::ResponseReader::readResponse()
{
    // recving from server the header for the length
    DBG("recving header");
    optStr header = net.recvAll(messaging::REQUEST_DATA_LENGTH_SIZE);
    if (!header)
        return std::nullopt;

    // extracting info from header
    optPrsdResp prsdRqst = messaging::ClientProtocol::parseHeader(header.value(), header->size());
    DBG("data size from header: " << prsdRqst->dataSize);

    // dont recv data if it isnt expcted.
    if (prsdRqst->dataSize == 0)
        return "";

    // recving data
    DBG("recving data from server...");
    optStr dataResp = net.recvAll(prsdRqst->dataSize);
    if (!dataResp)
        return std::nullopt;

    optPrsdResp refindPrsdRqst = messaging::ClientProtocol::parseData(
        std::move(*prsdRqst),
        *dataResp
    );
    return refindPrsdRqst->dataBuffer;
}
