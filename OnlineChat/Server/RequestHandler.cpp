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
	const auto decryptedMsg = AESWrapper::decryptWithKey(dbContent, sessionManager.getAESkey(sock));
	if (!decryptedMsg)
		throw AESSessionKeyError("decryption failed for send message request");
	const std::string payload = messaging::ServerProtocol::constructResponse(decryptedMsg.value(), Code::OK);
	DBG("sending this in getchat:" << payload);
	netIO.sendAll(sock, payload);
}

void sockets::server::RequestHandler::handleSendMessage(SOCKET sock, const messaging::ParsedRequest& parsedRq)
{
	DBG("sendMessage request called");
	// decrypts the message with the clients AES key
	const auto decryptedMsg = AESWrapper::decryptWithKey(parsedRq.dataBuffer, sessionManager.getAESkey(sock));
	if (!decryptedMsg)
		throw AESSessionKeyError("decryption failed for send message request");
	// logs formatted message to the database
	const std::string formattedMsg = reg.getUserName(sock) + ": " + decryptedMsg.value();
	dbManager.writeToDB(formattedMsg);

	// brodcasts the message
	broadcastHelper(formattedMsg);
}

void sockets::server::RequestHandler::handleRegister(SOCKET sock, const messaging::ParsedRequest& parsedRq)
{
	const auto decryptedUsername = AESWrapper::decryptWithKey(parsedRq.dataBuffer, sessionManager.getAESkey(sock));
	if (!decryptedUsername)
		throw AESSessionKeyError("decryption failed for send message request");
	// registering
	if (!reg.registerUserName(sock, decryptedUsername.value()))
		throw RegistryError("already registrated / username taken");

	// sends sucsess message
	DBG("registrartion sucsess");

	// formatting and ecnrypting 
	std::string formattedMsg = "registration successful as " + reg.getUserName(sock);
	const auto encryptedMsg = AESWrapper::encryptWithKey(formattedMsg, sessionManager.getAESkey(sock));
	if(!encryptedMsg)
		throw AESSessionKeyError("encryption failed during registration response");

	const std::string payload = messaging::ServerProtocol::constructResponse(encryptedMsg.value(), Code::OK);

	// sending
	netIO.sendAll(sock, payload);
}

void sockets::server::RequestHandler::handleRSAKey(SOCKET sock, const messaging::ParsedRequest& parsdRqst)
{
	// generates the AES key and caches it in the sesssion

	// generate the key
	const auto AESkey = AESWrapper::generateAESKey();
	if (!AESkey) 
		throw AESSessionKeyError("AES key generation failed");

	// encrypt the AES key with the clients RSA public key
	const auto encryptedAESkey = RSAWrapper::encryptWithPublicKey(AESkey.value(), parsdRqst.dataBuffer);
	if (!encryptedAESkey)
		throw RSAWrapperError("encryption of AES key failed");
	// log the aes key into the session manager
	if (!sessionManager.setAESkey(sock, AESkey.value()))
		throw AESSessionKeyError("AES key already set for this client, cannot overwrite");

	std::string payload = messaging::ServerProtocol::constructResponse(encryptedAESkey.value(), Code::AESKEY);
	netIO.sendAll(sock, payload);
}

void sockets::server::RequestHandler::handleDirectMessage(SOCKET sock, const messaging::ParsedRequest& parsedRq)
{
	const auto decryptedMsg = AESWrapper::decryptWithKey(parsedRq.dataBuffer, sessionManager.getAESkey(sock));
	if (!decryptedMsg)
		throw AESSessionKeyError("encryption of AES key failed");
	// verify sender is registered
	const std::string& senderUsername = reg.getUserName(sock);
	DBG("databuffer" << decryptedMsg.value());

	// verify recver exists
	if (!parsedRq.recver || reg.getSocket(parsedRq.recver.value()) == INVALID_SOCKET)
		throw UserNotFoundError("user not found");

	DBG("parsed DM data, target user: " << *decryptedMsg << ", message content: " << parsedRq.recver.value());

	// send DM
	SOCKET targetSock = reg.getSocket(parsedRq.recver.value());
	
	// formatting and encrypting the message
	const std::string formattedMsg = "(DM from " + senderUsername + "): " + decryptedMsg.value();
	const auto encryptedMsg = AESWrapper::encryptWithKey(formattedMsg, sessionManager.getAESkey(targetSock));
	if (!encryptedMsg)
		throw AESSessionKeyError("encryption of AES key failed");
	const std::string payload = messaging::ServerProtocol::constructResponse(encryptedMsg.value(), Code::OK);

	netIO.sendAll(targetSock, payload);
}

void sockets::server::RequestHandler::broadcastHelper(std::string_view msg)
{
	DBG("broadcasting message: " << msg);
	// takes a snapshot of all current conncted clients
	std::vector<SOCKET> clients = sessionManager.clientsSnapshot();
	// brodcasts to every registered user
	for(SOCKET s : clients)
	{	
		if(reg.isClientExist(s))
		{
			// encrypts the message with the clients AES key before sending
			const auto encryptedMsg = AESWrapper::encryptWithKey(msg, sessionManager.getAESkey(s));
			if(!encryptedMsg)
				throw AESSessionKeyError("encryption of AES key failed during broadcast");
			const std::string payload = messaging::ServerProtocol::constructResponse(encryptedMsg.value(), Code::OK);

			netIO.sendAll(s, payload);
		}
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