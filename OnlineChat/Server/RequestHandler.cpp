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
using Code = messaging::ResponseCode;

void sockets::server::RequestHandler::handleRequest(SOCKET sock, const messaging::ParsedRequest& parsedRq)
{
	// matching which request type was asked for 
	try
	{
		if (!isStatusOK(reg.isClientExist(sock), parsedRq))
			throw ProtocolError("status BAD");

		DBG("STATUS CODE OK");
		if (messaging::RequestType::GET_CHAT == parsedRq.requestType)
			RequestHandler::handleGetChat(sock);
		else if (messaging::RequestType::SEND_MESSAGE == parsedRq.requestType)
			RequestHandler::handleSendMessage(sock, parsedRq);
		else if (messaging::RequestType::REGISTER == parsedRq.requestType)
			RequestHandler::handleRegister(sock, parsedRq);
		else if (messaging::RequestType::DIRECT_MESSAGE == parsedRq.requestType)
			RequestHandler::handleDirectMessage(sock, parsedRq);
	}
	catch (const DataBaseError& e)
	{
		// database error
		DBG("DataBase error: " << e.what());
		const std::string payload = messaging::ServerProtocol::constructResponse(Code::DATABASE_ERR);
		netIO.sendAll(sock, payload);
	}
	catch (const ProtocolError& e)
	{
		// protocol error
		DBG("protocol error: " << e.what());
		const std::string payload = messaging::ServerProtocol::constructResponse(Code::PROTOCOL_ERR);
		netIO.sendAll(sock, payload);
	}
	catch (const UserNotFoundError& e)
	{
		// user not found err
		DBG("error handling request: " << e.what());
		const std::string payload = messaging::ServerProtocol::constructResponse(Code::USER_NOT_FOUND_ERR);
		netIO.sendAll(sock, payload);
	}
	catch (const NotRegisteredError& e)
	{
		// trying to request without being rergistreed
		DBG("error handling request: " << e.what());
		std::string payload = messaging::ServerProtocol::constructResponse(Code::NOT_REGISTER_ERR);
		netIO.sendAll(sock, payload);
	}
	catch (const RegistryError& e)
	{
		// errors that might occur from the registry
		DBG("error handling request: " << e.what());
		const std::string payload = messaging::ServerProtocol::constructResponse(Code::REGISTRY_ERR);
		netIO.sendAll(sock, payload);
	}
	catch (const std::exception& e)
	{
		// any other exceptions
		DBG("error handling request: " << e.what());
		const std::string payload = messaging::ServerProtocol::constructResponse(Code::PROTOCOL_ERR);
		netIO.sendAll(sock, payload);
	}
}

void sockets::server::RequestHandler::handleGetChat(SOCKET sock)
{
	DBG("getChat request called");
	const std::string dbContent = dbManager.readDB();
	const std::string payload = messaging::ServerProtocol::constructResponse(dbContent, Code::OK);
	DBG("sending this in getchat:" << payload);
	netIO.sendAll(sock, payload);
}

void sockets::server::RequestHandler::handleSendMessage(SOCKET sock, const messaging::ParsedRequest& parsedRq)
{
	DBG("sendMessage request called");

	// logs formatted message to the database
	const std::string msg = reg.getUserName(sock) + ": " + parsedRq.dataBuffer;
	dbManager.writeToDB(msg);

	// brodcasts the message
	broadcastHelper(msg);
}

bool sockets::server::RequestHandler::handleRegister(SOCKET sock, const messaging::ParsedRequest& parsedRq)
{
	// check if registration is valid
	if (!reg.registerUserName(sock, parsedRq.dataBuffer))
		throw RegistryError("already registrated / username taken");

	// sends sucsess message
	DBG("registrartion sucsess");
	std::string sendToUser = "registration successful as " + reg.getUserName(sock);
	const std::string payload = messaging::ServerProtocol::constructResponse(sendToUser, Code::OK);
	netIO.sendAll(sock, payload);
	return true;
}

void sockets::server::RequestHandler::handleDirectMessage(SOCKET sock, const messaging::ParsedRequest& parsedRq)
{
	// verify sender is registered
	const std::string& senderUsername = reg.getUserName(sock);
	DBG("databuffer" << parsedRq.dataBuffer);


	DBG("parsed DM data, target user: " << parsedRq.dataBuffer << ", message content: " << parsedRq.recver.value());

	// verify recver exists
	if (!parsedRq.recver || reg.getSocket(parsedRq.recver.value()) == INVALID_SOCKET)
		throw UserNotFoundError("user not found");

	// send DM
	SOCKET targetSock = reg.getSocket(parsedRq.recver.value());
	const std::string formattedMsg = "(DM from " + senderUsername + "): " + parsedRq.dataBuffer;
	const std::string payload = messaging::ServerProtocol::constructResponse(formattedMsg, Code::OK);
	netIO.sendAll(targetSock, payload);

}

void sockets::server::RequestHandler::broadcastHelper(std::string_view msg)
{
	DBG("broadcasting message: " << msg);
	const std::string payload = messaging::ServerProtocol::constructResponse(msg.data(), Code::OK);
	std::vector<SOCKET> clients = sessionManager.clientsSnapshot();
	// brodcasts to every registered user
	for(SOCKET s : clients)
	{	
		if(reg.isClientExist(s))
			netIO.sendAll(s, payload);
	}
}

bool sockets::server::RequestHandler::isStatusOK(
	bool isRegistered,
	const messaging::ParsedRequest& req)
{
	DBG(static_cast<int>(req.requestType) << ", " << req.dataSize << ", "
		<< req.dataBuffer << ", " << isRegistered);

	const bool hasPayload = (req.dataSize > 0 && !req.dataBuffer.empty());

	if (isRegistered)
	{
		switch (req.requestType)
		{
		case messaging::RequestType::GET_CHAT:
			return req.dataSize == 0;

		case messaging::RequestType::SEND_MESSAGE:
			return hasPayload;

		case messaging::RequestType::DIRECT_MESSAGE:
			// Ideally also require req.recver has value (parsed properly)
			return hasPayload && req.recver.has_value();

		default:
			return false;
		}
	}

	// not registered
	if (req.requestType == messaging::RequestType::REGISTER)
		return hasPayload;

	throw NotRegisteredError("not registered");
}