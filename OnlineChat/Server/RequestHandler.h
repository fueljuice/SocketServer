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
/**
 * @brief Handles request processing for the server
 * 
 * This class provides methods to handle different types of requests
 * including getting chat history, sending messages, user registration, and direct messaging.
 * All methods are static and stateless, requiring all necessary context to be passed as parameters.
 */
class RequestHandler
{
public:
	RequestHandler(
		INetworkIO& sender_p,
		IUserRegistry& reg_p,
		IdbManager& dbManager_p,
		ISessionManager& sesManager);

	void handleRequest(SOCKET sock, messaging::ParsedRequest& pr);


private:
	INetworkIO& netIO;
	IUserRegistry& reg;
	IdbManager& dbManager;
	ISessionManager& sessionManager;
	void handleGetChat(SOCKET sock);
	void handleSendMessage(SOCKET sock, messaging::ParsedRequest& pr);
	void handleDirectMessage(SOCKET sock, messaging::ParsedRequest& pr);
	bool handleRegister(SOCKET sock, messaging::ParsedRequest& pr);

	void broadcastHelper(std::string msg);

};
}

