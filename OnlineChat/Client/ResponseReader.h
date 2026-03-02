#pragma once
#include <string>
#include <optional>
#include <utility>
#include "../Protocol/ProtocolConstants.h"
#include "../Protocol/ClientProtocol.h"
#include "NetworkManager.h"

namespace Client
{

struct IResponseReader
{
    virtual ~IResponseReader() = default;
    virtual std::optional<std::pair<std::string, messaging::ResponseCode>> readResponse() = 0;
};
class ResponseReader : public IResponseReader
{
public:
    ResponseReader(INetworkManager& net);
    std::optional<std::pair<std::string, messaging::ResponseCode>> readResponse() override;
    
private:
    INetworkManager& net;
};
}
