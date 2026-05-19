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
		if (!isRequestAllowed(reg.isClientExist(sock),!sessionManager.getAESkey(sock).empty(), parsedRq))
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
		else if (messaging::RequestType::SEND_RSA_PKEY == parsedRq.requestType)
			RequestHandler::handleRSAKey(sock, parsedRq);
		else
			throw ProtocolError("invalid request type");
	}
	catch (const DataBaseError& e)
	{
		// database error
		DBG("DataBase error: " << e.what());
		const std::string payload = messaging::ServerProtocol::constructResponse(Code::DATABASE_ERR);
		netIO.sendAll(sock, AESWrapper::encryptWithPublicKey(sessionManager.getAESkey(sock), payload));
	}
	catch (const ProtocolError& e)
	{
		// protocol error
		DBG("protocol error: " << e.what());
		const std::string payload = messaging::ServerProtocol::constructResponse(Code::PROTOCOL_ERR);
		netIO.sendAll(sock, AESWrapper::encryptWithPublicKey(sessionManager.getAESkey(sock), payload));
	}
	catch (const UserNotFoundError& e)
	{
		// user not found err
		DBG("error handling request: " << e.what());
		const std::string payload = messaging::ServerProtocol::constructResponse(Code::USER_NOT_FOUND_ERR);
		netIO.sendAll(sock, AESWrapper::encryptWithPublicKey(sessionManager.getAESkey(sock), payload));
	}
	catch (const NotRegisteredError& e)
	{
		// trying to request without being rergistreed
		DBG("error handling request: " << e.what());
		std::string payload = messaging::ServerProtocol::constructResponse(Code::NOT_REGISTER_ERR);
		netIO.sendAll(sock, AESWrapper::encryptWithPublicKey(sessionManager.getAESkey(sock), payload));
	}
	catch (const RegistryError& e)
	{
		// errors that might occur from the registry
		DBG("error handling request: " << e.what());
		const std::string payload = messaging::ServerProtocol::constructResponse(Code::REGISTRY_ERR);
		netIO.sendAll(sock, AESWrapper::encryptWithPublicKey(sessionManager.getAESkey(sock), payload));
	}
	catch (const AESSessionKeyError& e)
	{
		// couldnt generate AES key or AES key already exists for this client
		DBG("error handling request: " << e.what());
		const std::string payload = messaging::ServerProtocol::constructResponse(Code::AESKEY_ERR);
		netIO.sendAll(sock, payload);
	}
	catch (const RSAWrapperError& e)
	{
		// the openSSL rsa wrapper failed
		DBG("error handling request: " << e.what());
		const std::string payload = messaging::ServerProtocol::constructResponse(Code::AESKEY_ERR);
		netIO.sendAll(sock, AESWrapper::encryptWithPublicKey(sessionManager.getAESkey(sock), payload));
	}
	catch (const std::exception& e)
	{
		// any other exceptions
		DBG("error handling request: " << e.what());
		const std::string payload = messaging::ServerProtocol::constructResponse(Code::PROTOCOL_ERR);
		netIO.sendAll(sock, AESWrapper::encryptWithPublicKey(sessionManager.getAESkey(sock), payload));
	}
}

void sockets::server::RequestHandler::handleGetChat(SOCKET sock)
{
	DBG("getChat request called");
	const std::string dbContent = dbManager.readDB();
	const std::string payload = messaging::ServerProtocol::constructResponse(dbContent, Code::OK);
	DBG("sending this in getchat:" << payload);
	netIO.sendAll(sock, AESWrapper::encryptWithPublicKey(sessionManager.getAESkey(sock), payload));
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

void sockets::server::RequestHandler::handleRegister(SOCKET sock, const messaging::ParsedRequest& parsedRq)
{
	// check if registration is valid
	if (!reg.registerUserName(sock, parsedRq.dataBuffer))
		throw RegistryError("already registrated / username taken");

	// sends sucsess message
	DBG("registrartion sucsess");
	std::string sendToUser = "registration successful as " + reg.getUserName(sock);
	const std::string payload = messaging::ServerProtocol::constructResponse(sendToUser, Code::OK);
	netIO.sendAll(sock, AESWrapper::encryptWithPublicKey(sessionManager.getAESkey(sock), payload));
}

void sockets::server::RequestHandler::handleRSAKey(SOCKET sock, const messaging::ParsedRequest& parsdRqst)
{
	// generates the AES key and caches it in the sesssion manager

	// generate the key
	const auto AESkey = AESWrapper::generateAESKey();
	if (!AESkey) 
		throw AESSessionKeyError("AES key generation failed");
	// cache the key
	if(!sessionManager.setAESkey(sock, AESkey.value())) 
		throw AESSessionKeyError("AES key already set for this client, cannot overwrite");

	// encrypts AES key with the clients RSA public key and sends it back to the client
	std::string encryptedAESkey = RSAWrapper::encryptWithPublicKey(AESkey.value(), parsdRqst.dataBuffer);
	if (encryptedAESkey.empty())
		throw RSAWrapperError("encryption of AES key failed");

	netIO.sendAll(sock, encryptedAESkey);
}

void sockets::server::RequestHandler::handleDirectMessage(SOCKET sock, const messaging::ParsedRequest& parsedRq)
{
	// verify sender is registered
	const std::string& senderUsername = reg.getUserName(sock);
	DBG("databuffer" << parsedRq.dataBuffer);

	// verify recver exists
	if (!parsedRq.recver || reg.getSocket(parsedRq.recver.value()) == INVALID_SOCKET)
		throw UserNotFoundError("user not found");

	DBG("parsed DM data, target user: " << parsedRq.dataBuffer << ", message content: " << parsedRq.recver.value());

	// send DM
	SOCKET targetSock = reg.getSocket(parsedRq.recver.value());
	const std::string formattedMsg = "(DM from " + senderUsername + "): " + parsedRq.dataBuffer;
	const std::string payload = messaging::ServerProtocol::constructResponse(formattedMsg, Code::OK);
	netIO.sendAll(targetSock, AESWrapper::encryptWithPublicKey(sessionManager.getAESkey(sock), payload));

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
			netIO.sendAll(s, AESWrapper::encryptWithPublicKey(sessionManager.getAESkey(s), payload));
	}
}

bool sockets::server::RequestHandler::isRequestAllowed(
	bool isRegistered,
	bool isEncrypted,
	const messaging::ParsedRequest& req)
{
	DBG(static_cast<int>(req.requestType) << ", " << req.dataSize << ", "
		<< req.dataBuffer << ", " << isRegistered);

	const bool hasPayload = (req.dataSize > 0 && !req.dataBuffer.empty());

	// must init encryption before any other request
	if (!isEncrypted)
	{
		if ((req.requestType == messaging::RequestType::SEND_RSA_PKEY && hasPayload))
			return true;
		throw ProtocolError("encryption not initialized");
	}

	// registered users
	if (isRegistered)
	{
		switch (req.requestType)
		{
		case messaging::RequestType::GET_CHAT:
			return req.dataSize == 0;

		case messaging::RequestType::SEND_MESSAGE:
			return hasPayload;

		case messaging::RequestType::DIRECT_MESSAGE:
			return hasPayload && req.recver.has_value();

		default:
			throw RegistryError("registry error");
		}
	}

	// not registered
	if (req.requestType == messaging::RequestType::REGISTER)
		return hasPayload;

	throw NotRegisteredError("not registered");
}