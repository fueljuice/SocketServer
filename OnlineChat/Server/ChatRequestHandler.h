#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <iostream>
#include <WinSock2.h>
#include <memory>
#include <utility>

#include "../Protocol/ParsedRequest.h"
#include "./NetworkIO.h"
#include "./UserRegistry.h"
#include "./DataBaseManager.h"
#include "./SessionManager.h"
namespace sockets::server
{
/**
 * @brief Handles chat request processing for the server
 * 
 * This class provides static methods to handle different types of chat requests
 * including getting chat history, sending messages, user registration, and direct messaging.
 * All methods are static and stateless, requiring all necessary context to be passed as parameters.
 */
class ChatRequestHandler
{
public:
	ChatRequestHandler(
		std::unique_ptr<NetworkIO>& sender_p,
		std::unique_ptr<UserRegistry>& reg_p,
		std::unique_ptr<DataBaseManager>& dbManager_p,
		std::unique_ptr<SessionManager>& sesManager);

	void handleRequest(SOCKET sock, messaging::ParsedRequest& pr);


private:
	std::unique_ptr<NetworkIO>& netIO;
	std::unique_ptr<UserRegistry>& reg;
	std::unique_ptr<DataBaseManager>& dbManager;
	std::unique_ptr<SessionManager>& sessionManager;

	void handleGetChat(SOCKET sock);
	void handleSendMessage(SOCKET sock, messaging::ParsedRequest& pr);
	void handleDirectMessage(SOCKET sock, messaging::ParsedRequest& pr);
	bool handleRegister(SOCKET sock, messaging::ParsedRequest& pr);

	void broadcastHelper(std::string msg);

};
}
