#include "ResponseHandler.h"



ResponseHandler::ResponseHandler(IAESWrapper& aes, IRSAWrapper& rsa, IGuiManager& gui)
    :
    aes(aes),
    rsa(rsa),
    gui(gui)
{
}

void ResponseHandler::handleResponse(std::string_view data, messaging::ResponseCode code)
{
    // case aes key
	if (code == messaging::ResponseCode::AESKEY)
    {
        if(!handleAESKeyResponse(data))
        {
            gui.logScreen("", messageForCode(messaging::ResponseCode::AESKEY_ERR));
            return;
        }
		gui.logScreen("", messageForCode(messaging::ResponseCode::AESKEY));
        return;
    }
	auto decrypted = aes.decrypt(data);
    if (!decrypted)
        return;
    gui.logScreen(decrypted.value(), messageForCode(code));
}

std::string ResponseHandler::messageForCode(messaging::ResponseCode code)
{
    using RC = messaging::ResponseCode;

    switch (code)
    {
    case RC::NO_RESPONSE:
        return "Couldn’t reach the server. Check your connection and try again.";

    case RC::OK:
        return "";

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

    case RC::REGISTRY_ERR:
        return "registration error. username alreay exists";

	case RC::AESKEY_ERR:
		return "Failed to establish a secure connection. Please try again later."; 

    default:
        return "Unknown error.";
    }
}

bool ResponseHandler::handleAESKeyResponse(std::string_view data)
{
	// sets the AES key by decrypting the received data with the RSA private key
    if (data.empty())
        return false;

    const auto aesKey = rsa.decrypt(data);

    if (!aesKey)
        return false;

    aes.setKey(aesKey.value());
    return true;
}
