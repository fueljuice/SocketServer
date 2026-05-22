#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <iostream>
#include <WinSock2.h>
#include <memory>
#include <utility>
#include <functional>
#include <vector>

#include "NetworkIO.h"
#include "UserRegistry.h"
#include "DataBaseManager.h"
#include "SessionManager.h"


#include "../Security/RSAWrapper.h"
#include "../Security/AESWrapper.h"
#include "../Protocol/ParsedRequest.h"
#include "../Protocol/ServerProtocol.h"
 
namespace sockets::server
{
struct IRequestHandler
{
public:
	virtual ~IRequestHandler() = default;
	virtual void handleRequest(SOCKET sock, const messaging::ParsedRequest& parsdRqst) = 0;
	virtual void onClientDisconnect(SOCKET sock) = 0;
};
class RequestHandler : public IRequestHandler
{
public:
	RequestHandler(
		INetworkIO& sender_p,
		IUserRegistry& reg_p,
		IdbManager& dbManager_p,
		ISessionManager& sesManager);

	void handleRequest(SOCKET sock, const messaging::ParsedRequest& parsdRqst);
	void onClientDisconnect(SOCKET sock);


private:
	INetworkIO& netIO;
	IUserRegistry& reg;
	IdbManager& dbManager;
	ISessionManager& sessionManager;

	// request specific handling
	void handleGetChat(SOCKET sock);
	void handleSendMessage(SOCKET sock, const messaging::ParsedRequest& parsdRqst);
	void handleDirectMessage(SOCKET sock, const messaging::ParsedRequest& parsdRqst);
	void handleRegister(SOCKET sock, const messaging::ParsedRequest& parsdRqst);
	void handleRSAKey(SOCKET sock, const messaging::ParsedRequest& parsdRqst);
	void broadcastHelper(std::string_view msg);

	void onClientRegister(std::string_view username);
	void sendResponse(SOCKET sock, std::string_view plainBody, messaging::ResponseCode code);
	bool isRequestAllowed(bool isRegistered, bool isEncrypted, const messaging::ParsedRequest& parsdRqst);
};
}

