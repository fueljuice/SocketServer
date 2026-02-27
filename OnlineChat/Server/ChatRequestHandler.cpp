#include "ChatRequestHandler.h"

#include "../Protocol/ProtocolConstants.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

sockets::server::ChatRequestHandler::ChatRequestHandler(
	std::unique_ptr<NetworkIO>& sender_p, 
	std::unique_ptr<UserRegistry>& reg_p,
	std::unique_ptr<DataBaseManager>& dbManager_p,
	std::unique_ptr<SessionManager>& sessionManager_p)
	:
	netIO(sender_p),
	reg(reg_p),
	dbManager(dbManager_p),
	sessionManager(sessionManager_p)
	{}

void sockets::server::ChatRequestHandler::handleRequest(SOCKET sock, messaging::ParsedRequest& parsedRq)
{

	if (!messaging::ServerProtocol::isStatusOK(parsedRq, sock))
	{
		DBG("STATUS CODE BAD 404");
		return;
	}

	DBG("STATUS CODE OK 200");
	// matching which request type was asked for 
	try
	{
		if (messaging::ActionType::GET_CHAT == parsedRq.requestType)
			ChatRequestHandler::handleGetChat(sock);
		else if (messaging::ActionType::SEND_MESSAGE == parsedRq.requestType)
			ChatRequestHandler::handleSendMessage(sock, parsedRq);
		else if (messaging::ActionType::REGISTER == parsedRq.requestType)
			ChatRequestHandler::handleRegister(sock, parsedRq);
		else if (messaging::ActionType::DIRECT_MESSAGE == parsedRq.requestType)
			ChatRequestHandler::handleDirectMessage(sock, parsedRq);
	}
	catch (const DataBaseError& e)
	{
		// ADD ERROR CODE 
		DBG("DataBase error: " << e.what());
		std::string payload = messaging::ServerProtocol::constructResponse(e.what());
		netIO->sendAll(sock, payload);
	}
	catch (const std::exception& e)
	{
		// ADD ERROR CODE 
		DBG("error handling request: " << e.what());
		std::string payload = messaging::ServerProtocol::constructResponse("handling error");
		netIO->sendAll(sock, payload);
	}
}

void sockets::server::ChatRequestHandler::handleGetChat(SOCKET sock)
{
	DBG("getChat request called");
	std::string dbContent = dbManager->readDB();
	std::string payload = messaging::ServerProtocol::constructResponse(dbContent);
	netIO->sendAll(sock, payload);
}

void sockets::server::ChatRequestHandler::handleSendMessage(SOCKET sock, messaging::ParsedRequest& parsedRq)
{
	DBG("sendMessage request called");

	// cant send message before registering
	if (!reg->isClientExist(sock))
	{
		// ADD ERROR CODE 
		std::string errMsg = messaging::ServerProtocol::constructResponse("please register");
		netIO->sendAll(sock, errMsg);
		return;
	}

	// logs formatted message to the database
	std::string msg = reg->getUserName(sock) + ": " + parsedRq.dataBuffer;
	dbManager->writeToDB(parsedRq.dataBuffer);

	// brodcasts the message
	broadcastHelper(parsedRq.dataBuffer);
}

bool sockets::server::ChatRequestHandler::handleRegister(SOCKET sock, messaging::ParsedRequest& parsedRq)
{
	// check if registration is valid
	if(!reg->registerUserName(sock, parsedRq.dataBuffer))
	{
		DBG("registration failed for user: " << parsedRq.dataBuffer);
		std::string errMsg = messaging::ServerProtocol::constructResponse("registration failed.username may be taken.");
		netIO->sendAll(sock, errMsg);
		return false;
	}

	// sends sucsess message
	DBG("registrartion sucsess");
	std::string payload = messaging::ServerProtocol::constructResponse("registration successful.");
	netIO->sendAll(sock, payload);
	return true;
}
void sockets::server::ChatRequestHandler::broadcastHelper(std::string msg)
{
	DBG("broadcasting message: " << msg);
	std::string payload = messaging::ServerProtocol::constructResponse(msg);
	size_t idx = 0;
	SOCKET curSock = sessionManager->getClientSocketByIndex(idx);
	while(true)
	{
		SOCKET curSock = sessionManager->getClientSocketByIndex(idx);
		if (curSock == INVALID_SOCKET)
			break;
		netIO->sendAll(curSock, payload);
		idx++;
	}

}

void sockets::server::ChatRequestHandler::handleDirectMessage(SOCKET sock, messaging::ParsedRequest& parsedRq)
{
	// verify sender is registered
	const std::string& senderUsername = reg->getUserName(sock);
	if (senderUsername.empty())
	{
		DBG("client not registered, cannot send DM");
		std::string payload = messaging::ServerProtocol::constructResponse("please register before sending direct messages.");
		netIO->sendAll(sock, payload);
		return;
	}

	// seperate recver name from message
	std::pair<std::string, std::string> dmData = messaging::ServerProtocol::parseDirectMessage(parsedRq.dataBuffer);
	std::string_view targetUserName = dmData.first;
	std::string_view messageContent = dmData.second;

	// verify recver exists
	if (reg->getSocket(targetUserName) == INVALID_SOCKET)
	{
		std::string payload = messaging::ServerProtocol::constructResponse("user not found.");
		netIO->sendAll(sock, payload);
		return;
	}
	// send dm
	SOCKET targetSock = reg->getSocket(targetUserName);
	std::string formattedMsg = "(DM from " + senderUsername + "): " + messageContent.data();
	std::string payload = messaging::ServerProtocol::constructResponse(formattedMsg);
	netIO->sendAll(sock, payload);

}
