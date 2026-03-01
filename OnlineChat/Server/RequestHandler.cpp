#include "RequestHandler.h"

#include "../Protocol/ProtocolConstants.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

sockets::server::RequestHandler::RequestHandler(
	INetworkIO& sender_p, 
	IUserRegistry& reg_p,
	IdbManager& dbManager_p,
	ISessionManager& sessionManager_p)
	:
	netIO(sender_p),
	reg(reg_p),
	dbManager(dbManager_p),
	sessionManager(sessionManager_p)
	{}

void sockets::server::RequestHandler::handleRequest(SOCKET sock, messaging::ParsedRequest& parsedRq)
{

	if (!messaging::ServerProtocol::isStatusOK(parsedRq, reg.isClientExist(sock)))
	{
		DBG("STATUS CODE BAD 404");
		return;
	}

	DBG("STATUS CODE OK 200");
	// matching which request type was asked for 
	try
	{
		if (messaging::ActionType::GET_CHAT == parsedRq.requestType)
			RequestHandler::handleGetChat(sock);
		else if (messaging::ActionType::SEND_MESSAGE == parsedRq.requestType)
			RequestHandler::handleSendMessage(sock, parsedRq);
		else if (messaging::ActionType::REGISTER == parsedRq.requestType)
			RequestHandler::handleRegister(sock, parsedRq);
		else if (messaging::ActionType::DIRECT_MESSAGE == parsedRq.requestType)
			RequestHandler::handleDirectMessage(sock, parsedRq);
	}
	catch (const DataBaseError& e)
	{
		// ADD ERROR CODE 
		DBG("DataBase error: " << e.what());
		std::string payload = messaging::ServerProtocol::constructResponse(e.what());
		netIO.sendAll(sock, payload);
	}
	catch (const std::exception& e)
	{
		// ADD ERROR CODE 
		DBG("error handling request: " << e.what());
		std::string payload = messaging::ServerProtocol::constructResponse("handling error");
		netIO.sendAll(sock, payload);
	}
}

void sockets::server::RequestHandler::handleGetChat(SOCKET sock)
{
	DBG("getChat request called");
	std::string dbContent = dbManager.readDB();
	std::string payload = messaging::ServerProtocol::constructResponse(dbContent);
	DBG("sending this in getchat:" << payload);
	netIO.sendAll(sock, payload);
}

void sockets::server::RequestHandler::handleSendMessage(SOCKET sock, messaging::ParsedRequest& parsedRq)
{
	DBG("sendMessage request called");

	// logs formatted message to the database
	std::string msg = reg.getUserName(sock) + ": " + parsedRq.dataBuffer;
	dbManager.writeToDB(parsedRq.dataBuffer);

	// brodcasts the message
	broadcastHelper(parsedRq.dataBuffer);
}

bool sockets::server::RequestHandler::handleRegister(SOCKET sock, messaging::ParsedRequest& parsedRq)
{
	// check if registration is valid
	if(!reg.registerUserName(sock, parsedRq.dataBuffer))
	{
		DBG("registration failed for user: " << parsedRq.dataBuffer);
		std::string errMsg = messaging::ServerProtocol::constructResponse("registration failed.username may be taken.");
		netIO.sendAll(sock, errMsg);
		return false;
	}

	// sends sucsess message
	DBG("registrartion sucsess");
	std::string payload = messaging::ServerProtocol::constructResponse("registration successful.");
	netIO.sendAll(sock, payload);
	return true;
}

void sockets::server::RequestHandler::handleDirectMessage(SOCKET sock, messaging::ParsedRequest& parsedRq)
{
	// verify sender is registered
	const std::string& senderUsername = reg.getUserName(sock);
	DBG("databuffer" << parsedRq.dataBuffer);


	DBG("parsed DM data, target user: " << parsedRq.dataBuffer << ", message content: " << parsedRq.recver.value());

	// verify recver exists
	if (!parsedRq.recver|| reg.getSocket(parsedRq.recver.value()) == INVALID_SOCKET)
	{
		std::string payload = messaging::ServerProtocol::constructResponse("user not found.");
		netIO.sendAll(sock, payload);
		return;
	}

	// send DM
	SOCKET targetSock = reg.getSocket(parsedRq.recver.value());
	std::string formattedMsg = "(DM from " + senderUsername + "): " + parsedRq.dataBuffer;
	std::string payload = messaging::ServerProtocol::constructResponse(formattedMsg);
	netIO.sendAll(targetSock, payload);

}

void sockets::server::RequestHandler::broadcastHelper(std::string msg)
{
	DBG("broadcasting message: " << msg);
	std::string payload = messaging::ServerProtocol::constructResponse(msg);
	std::vector<SOCKET> clients = sessionManager.clientsSnapshot();
	for(SOCKET s : clients)
		netIO.sendAll(s, payload);
}