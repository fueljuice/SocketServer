#include "guiManager.h"

void GuiManager::logScreen(std::string msg, messaging::ResponseCode code)
{
    auto errMsg = messageForCode(code);
    if (errMsg)
        std::cout << "[Error]:" << *errMsg << std::endl;
    std::cout << "[New]... " + msg << std::endl;
}

std::optional<std::string> GuiManager::messageForCode(messaging::ResponseCode code)
{
    using RC = messaging::ResponseCode;

    switch (code)
    {
    case RC::NO_RESPONSE:
        return "Couldn’t reach the server. Check your connection and try again.";

    case RC::OK:
        return std::nullopt;

    case RC::ABORTED_ERR:
        return "The operation was cancelled.";

    case RC::NOT_REGISTER_ERR:
        return "You must register first. Use /reg <username>.";

    case RC::USER_NOT_FOUND_ERR:
        return "user not found.";

    case RC::DATABASE_ERR:
        return "server database error. Please try again later.";

    case RC::PROTOCOL_ERR:
        return "server failed to process the request. maybe outdated client version?";

    default:
        return "Unknown error.";
    }
}
