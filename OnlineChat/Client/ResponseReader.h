#pragma once
#include <string>
#include <optional>
#include "../Protocol/ClientProtocol.h"
#include "NetworkManager.h"

namespace Client
{
class ResponseReader
{
public:
    ResponseReader(INetworkManager& net);
    std::optional<std::string> readResponse();
    
private:
    INetworkManager& net;
};
}
