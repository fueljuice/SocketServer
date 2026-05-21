#pragma once
#include <string>
#include <string_view>


#include "GuiManager.h"
#include "../Security/AESWrapper.h"
#include "../Security/RSAWrapper.h"
#include "../Protocol/ProtocolConstants.h"


class IResponseHandler
{
public:
    virtual ~IResponseHandler() = default;

    virtual void handleResponse(
        std::string_view data,
        messaging::ResponseCode code) = 0;
};

class ResponseHandler : public IResponseHandler
{
public:
    ResponseHandler(
        IAESWrapper& aes,
        IRSAWrapper& rsa,
        IGuiManager& gui);

    void handleResponse(
        std::string_view data,
        messaging::ResponseCode code) override;

private:
    std::string messageForCode(messaging::ResponseCode code);
    bool handleAESKeyResponse(std::string_view data);

private:
    IAESWrapper& aes;
    IRSAWrapper& rsa;
    IGuiManager& gui;
};