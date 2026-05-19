#pragma once
#include <string>
#include <optional>
#include <utility>

#include "NetworkManager.h"
#include "GuiManager.h"
#include "../Protocol/ProtocolConstants.h"
#include "../Protocol/ClientProtocol.h"

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
