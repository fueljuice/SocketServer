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

#include "../Protocol/ParsedRequest.h"
#include "../Protocol/ServerProtocol.h"
 
namespace sockets::server
{
struct IRequestHandler
{
public:
	virtual void handleRequest(SOCKET sock, const messaging::ParsedRequest& parsdRqst) = 0;
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


private:
	INetworkIO& netIO;
	IUserRegistry& reg;
	IdbManager& dbManager;
	ISessionManager& sessionManager;
	void handleGetChat(SOCKET sock);
	void handleSendMessage(SOCKET sock, const messaging::ParsedRequest& parsdRqst);
	void handleDirectMessage(SOCKET sock, const messaging::ParsedRequest& parsdRqst);
	bool handleRegister(SOCKET sock, const messaging::ParsedRequest& parsdRqst);
	bool isStatusOK(bool isRegistered, const messaging::ParsedRequest& parsdRqst);
	void broadcastHelper(std::string_view msg);

};
}

