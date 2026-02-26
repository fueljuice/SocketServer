#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <functional>
#include <cstring>
#include <iostream>
#include <iterator>

#include <WinSock2.h>
#include "../Protocol/ParsedRequest.h"

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
	/**
	 * @brief Handles GET_CHAT request - sends entire chat history to client
	 * 
	 * Reads the complete chat history from the database file and sends it to the requesting client.
	 * The message is truncated if it exceeds MAX_MESSAGE_LENGTH.
	 * 
	 * @param clientSocket Socket of the requesting client
	 * @param pr Parsed request containing request metadata
	 * @param dbFile Reference to the database file stream
	 * @param fileMutex Mutex for thread-safe file access
	 * @param sendAll Function to send data to client
	 */
	static void handleGetChat(
		SOCKET clientSocket,
		messaging::ParsedRequest& pr,
		std::fstream& dbFile,
		std::mutex& fileMutex,
		const std::function<bool(SOCKET, const char*, int)>& sendAll
	);
	/**
	 * @brief Handles SEND_MESSAGE request - broadcasts message to all clients
	 * 
	 * Validates that the client is registered, then writes the message to the database
	 * and broadcasts it to all connected clients. Message format: "username: message"
	 * 
	 * @param clientSocket Socket of the sending client
	 * @param pr Parsed request containing message data
	 * @param dbFile Reference to the database file stream
	 * @param fileMutex Mutex for thread-safe file access
	 * @param nameMapMutex Mutex for thread-safe username map access
	 * @param clientsNameMap Map of sockets to usernames
	 * @param broadcast Function to broadcast message to all clients
	 * @param sendAll Function to send data to client
	 */
	static void handleSendMessage(
		SOCKET clientSocket,
		messaging::ParsedRequest& pr,
		std::fstream& dbFile,
		std::mutex& fileMutex,
		std::mutex& nameMapMutex,
		std::unordered_map<SOCKET, std::string>& clientsNameMap,
		const std::function<void(const char*, int)>& broadcast,
		const std::function<bool(SOCKET, const char*, int)>& sendAll
	);

	/**
	 * @brief Handles REGISTER request - registers a new client username
	 * 
	 * Validates that the client is not already registered and that the requested username
	 * is not taken by another client. If valid, adds the client to the username map.
	 * 
	 * @param clientSocket Socket of the registering client
	 * @param pr Parsed request containing desired username in dataBuffer
	 * @param nameMapMutex Mutex for thread-safe username map access
	 * @param clientsNameMap Map of sockets to usernames
	 * @param sendAll Function to send data to client
	 */
	static bool handleRegister(
		SOCKET clientSocket,
		messaging::ParsedRequest& pr,
		std::mutex& nameMapMutex,
		std::unordered_map<SOCKET, std::string>& clientsNameMap,
		const std::function<bool(SOCKET, const char*, int)>& sendAll
	);
	/**
	 * @brief Handles DIRECT_MESSAGE request - sends private message to specific user
	 * 
	 * Sends a private message to a specific user. The message data must be in format:
	 * "{targetUsername}:{message}". Validates that sender is registered and target user exists.
	 * The recipient receives: "(DM from {senderUsername}): {message}"
	 * 
	 * @param clientSocket Socket of the sending client
	 * @param pr Parsed request containing message data in "{targetUsername}:{message}" format
	 * @param nameMapMutex Mutex for thread-safe username map access
	 * @param clientsNameMap Map of sockets to usernames
	 * @param sendAll Function to send data to client
	 */
	static void handleDirectMessage(
		SOCKET clientSocket,
		messaging::ParsedRequest& pr,
		std::mutex& nameMapMutex,
		std::unordered_map<SOCKET, std::string>& clientsNameMap,
		const std::function<bool(SOCKET, const char*, int)>& sendAll
	);
};
}
