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
		sendResponse(sock, "", Code::DATABASE_ERR);
	}
	catch (const ProtocolError& e)
	{
		// protocol error
		DBG("protocol error: " << e.what());
		sendResponse(sock, "", Code::PROTOCOL_ERR);
	}
	catch (const UserNotFoundError& e)
	{
		// user not found err
		DBG("error handling request: " << e.what());
		sendResponse(sock, "", Code::USER_NOT_FOUND_ERR);
	}
	catch (const NotRegisteredError& e)
	{
		// trying to request without being rergistreed
		DBG("error handling request: " << e.what());
		sendResponse(sock, "", Code::NOT_REGISTER_ERR);
	}
	catch (const RegistryError& e)
	{
		// errors that might occur from the registry
		DBG("error handling request: " << e.what());
		sendResponse(sock, "", Code::REGISTRY_ERR);
	}
	catch (const AESSessionKeyError& e)
	{
		// couldnt generate AES key or AES key already exists for this client
		DBG("error handling request: " << e.what());
		sendResponse(sock, "", Code::AESKEY_ERR);
	}
	catch (const RSAWrapperError& e)
	{
		// the openSSL rsa wrapper failed
		DBG("error handling request: " << e.what());
		sendResponse(sock, "", Code::RSAKEY_ERR);
	}
	catch (const AlreadyRequested& e)
	{
		// the client already made this request and cant make it again
		DBG("error handling request: " << e.what());
		sendResponse(sock, "", Code::ALREADY_REQUESTED_ERR);
		return;
	}
	catch (const std::exception& e)
	{
		// any other exceptions
		DBG("error handling request: " << e.what());
		sendResponse(sock, "", Code::PROTOCOL_ERR);
		return;
	}
}

void sockets::server::RequestHandler::handleGetChat(SOCKET sock)
{
	// read the chatlog
	DBG("getChat request called");
	const std::string dbContent = dbManager.readDB();
	
	// send it
	sendResponse(sock, dbContent, Code::OK);
}

void sockets::server::RequestHandler::handleSendMessage(SOCKET sock, const messaging::ParsedRequest& parsedRq)
{
	// decrypting the message
	DBG("sendMessage request called");
	const auto decryptedMsg = AESWrapper::decryptWithKey(parsedRq.dataBuffer, sessionManager.getAESkey(sock));
	if (!decryptedMsg)
		throw AESSessionKeyError("decryption failed for send message request");

	// logs formatted message to the database (chat log)
	const std::string formattedMsg = reg.getUserName(sock) + ": " + decryptedMsg.value();
	dbManager.writeToDB(formattedMsg);

	// brodcasts the message
	broadcastHelper(formattedMsg);
}

void sockets::server::RequestHandler::handleRegister(SOCKET sock, const messaging::ParsedRequest& parsedRq)
{

	DBG("SERVER KEY SIZE: " << sessionManager.getAESkey(sock).size());
	DBG("SERVER KEY HEX: " << sessionManager.getAESkey(sock));
	DBG("SERVER CIPHER SIZE: " << parsedRq.dataBuffer.size());
	DBG("SERVER CIPHER HEX: " << parsedRq.dataBuffer);

	// decrypting the client's username
	const auto decryptedUsername = AESWrapper::decryptWithKey(parsedRq.dataBuffer, sessionManager.getAESkey(sock));
	if (!decryptedUsername)
		throw AESSessionKeyError("decryption failed for send message request");

	// registering
	if (!reg.registerUserName(sock, decryptedUsername.value()))
		throw RegistryError("already registrated / username taken");

	// sends sucsess message
	DBG("registrartion sucsess");

	// formatting and sending 
	std::string formattedMsg = "registration successful as " + reg.getUserName(sock);
	sendResponse(sock, formattedMsg, Code::OK);
}

void sockets::server::RequestHandler::handleRSAKey(SOCKET sock, const messaging::ParsedRequest& parsdRqst)
{
	DBG("sending rsa key");

	// generate an AES key
	const auto AESkey = AESWrapper::generateAESKey();
	if (!AESkey)
		throw AESSessionKeyError("AES key generation failed");

	// encrypt the AES key with the clients RSA public key
	const auto encryptedAESkey = RSAWrapper::encryptWithPublicKey(AESkey.value(), parsdRqst.dataBuffer);
	if (!encryptedAESkey)
		throw RSAWrapperError("encryption of AES key with RSA public key failed");

	// log the aes key into the session manager
	if (!sessionManager.setAESkey(sock, AESkey.value()))
		throw AESSessionKeyError("AES key already set for this client, cannot overwrite");

	// send the AES key encrypted
	const std::string payload = messaging::ServerProtocol::constructResponse(encryptedAESkey.value(), Code::AESKEY);
	netIO.sendAll(sock, payload);
}


void sockets::server::RequestHandler::handleDirectMessage(SOCKET sock, const messaging::ParsedRequest& parsedRq)
{
	// decrypting the message
	const auto decryptedMsg = AESWrapper::decryptWithKey(parsedRq.dataBuffer, sessionManager.getAESkey(sock));
	if (!decryptedMsg)
		throw AESSessionKeyError("encryption of AES key failed");

	// getting sender username
	const std::string& senderUsername = reg.getUserName(sock);
	DBG("databuffer" << decryptedMsg.value());

	// verify recver avaiable
	if (!parsedRq.recver)
		throw UserNotFoundError("receiver missing");
	SOCKET targetSock = reg.getSocket(parsedRq.recver.value());
	if (targetSock == INVALID_SOCKET)
		throw UserNotFoundError("user not found");

	DBG("parsed DM data, target user: " << *decryptedMsg << ", message content: " << parsedRq.recver.value());
	
	// formatting and sending the message to the reciver
	const std::string messageToReciever = "(DM from " + senderUsername + "): " + decryptedMsg.value();
	sendResponse(sock, messageToReciever, messaging::ResponseCode::OK);

	// formatting and sending the message to the sender
	const std::string messageToSender = "(You've sent a DM to " + parsedRq.recver.value() + "): " + decryptedMsg.value();
	sendResponse(targetSock, messageToSender, messaging::ResponseCode::OK);
}

void sockets::server::RequestHandler::broadcastHelper(std::string_view msg)
{
	DBG("broadcasting message: " << msg);
	// takes a snapshot of all current conncted clients
	std::vector<SOCKET> clients = sessionManager.clientsSnapshot();

	// brodcasts to every registered user
	for(SOCKET s : clients)
	{	
		if (reg.isClientExist(s))
			sendResponse(s, msg, messaging::ResponseCode::OK);
	}
}


void sockets::server::RequestHandler::sendResponse(SOCKET sock, std::string_view plainBody, messaging::ResponseCode code)
{
	// get AES key
	const std::string aesKey = sessionManager.getAESkey(sock);
	std::string bodyToSend;

	// send unencrypted if theres no key
	if (aesKey.empty())
	{
		const std::string payload =
			messaging::ServerProtocol::constructResponse(std::string(plainBody), code);

		netIO.sendAll(sock, payload);
		return;
	}

	// encrypted the body (if exists)
	if (!plainBody.empty())
	{
		auto encryptedBody =
			AESWrapper::encryptWithKey(plainBody, aesKey);

		if (!encryptedBody)
			throw AESSessionKeyError("failed to encrypt response body");

		bodyToSend = std::move(encryptedBody.value());
	}

	// construct and send the response
	const std::string payload =
		messaging::ServerProtocol::constructResponse(bodyToSend, code);
	if (!netIO.sendAll(sock, payload))
		DBG("failed to send response to client due to no connection."); 
}

bool sockets::server::RequestHandler::isRequestAllowed(bool isRegistered, bool isEncrypted, const messaging::ParsedRequest& req)
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
			throw AlreadyRequested("request not allowed in current state");
		}
	}

	// not registered
	if (req.requestType == messaging::RequestType::REGISTER)
		return hasPayload;

	throw NotRegisteredError("not registered");
}
