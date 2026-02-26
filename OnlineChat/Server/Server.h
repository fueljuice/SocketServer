#pragma once

#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <fstream>
#include <unordered_map>
#include <array>

#include "./data/clientSocketData.h"
#include "AbstractServer.h"
#include "../Protocol/ServerProtocol.h"



namespace sockets::server
{

/**
 * @brief Main chat server implementation
 * 
 * This class implements the core chat server functionality including client connection management,
 * request handling, and message broadcasting. It extends AbstractServer and provides a complete
 * multithreaded server that can handle multiple concurrent clients.
 * 
 * Features:
 * - Multi-threaded client handling
 * - Thread-safe client management
 * - File-based chat history persistence
 * - User registration and authentication
 * - Message broadcasting and direct messaging
 * - Graceful shutdown handling
 */
class Server final: public AbstractServer
{

public:
	/**
	 * @brief Starts the server and begins accepting connections
	 * 
	 * Starts the listening socket and begins accepting client connections.
	 * Each client is handled in a separate thread for concurrent processing.
	 */
	void launch() override;
	
	/**
	 * @brief Stops the server and cleans up resources
	 * 
	 * Stops accepting new connections, closes all client sockets,
	 * joins all client threads, and cleans up resources.
	 */
	void stop() override;
	
	/**
	 * @brief Constructs a new Server instance
	 * 
	 * @param domain Socket domain (AF_INET, AF_INET6, etc.)
	 * @param service Socket service type (SOCK_STREAM, etc.)
	 * @param protocol Socket protocol (IPPROTO_TCP, etc.)
	 * @param port Port number to listen on
	 * @param network_interface Network interface to bind to
	 * @param backlog Maximum number of pending connections
	 */
	Server(int domain, int service, int protocol,
		int port, u_long network_interaface, int backlog);
	
	/**
	 * @brief Destructor - ensures clean shutdown
	 * 
	 * Automatically stops the server if still running and closes the database file.
	 */
	~Server();

private:
	/** @brief Atomic flag to control server running state */
	std::atomic_bool running{ false };
	
	/** @brief File stream for chat history database */
	std::fstream dbFile;
	
	/** @brief Mutex to prevent concurrent sends to same client */
	std::mutex sendMutex;
	
	/** @brief Mutex to protect client vector during add/remove operations */
	std::mutex clientVectorMutex;
	
	/** @brief Mutex to protect file operations */
	std::mutex fileMutex;
	
	/** @brief Mutex to protect username map operations */
	std::mutex nameMapMutex;
	
	/** @brief Vector of client handler threads */
	std::vector<std::thread> clientThreads;
	
	/** @brief Vector of connected client data (shared ownership) */
	std::vector<std::shared_ptr<data::ClientSocketData>> clientVector;
	
	/** @brief Maps client sockets to registered usernames */
	std::unordered_map<SOCKET, std::string> clientsNameMap;


	/**
	 * @brief Accepts incoming client connections
	 * 
	 * Runs in a loop accepting new client connections and creating
	 * client data structures for each accepted connection.
	 */
	void acceptConnection();
	
	/**
	 * @brief Creates a new thread to handle a client
	 * 
	 * @param client Shared pointer to client socket data
	 */
	void openThreadForClient(std::shared_ptr<data::ClientSocketData> client);

	/**
	 * @brief Handles communication with a single client
	 * 
	 * Runs in a separate thread for each client. Receives requests,
	 * parses headers and data, and dispatches to appropriate handlers.
	 * 
	 * @param client Shared pointer to client socket data
	 */
	void handleConnection(std::shared_ptr<data::ClientSocketData> client);
	
	/**
	 * @brief Processes parsed client requests and dispatches to handlers
	 * 
	 * Parses the complete request (header + data) and routes it to the
	 * appropriate handler based on request type.
	 * 
	 * @param client Client making the request
	 * @param pr Parsed request containing header and data
	 */
	void respondToClient(std::shared_ptr<data::ClientSocketData> client, messaging::ParsedRequest& pr);

	/**
	 * @brief Broadcasts a message to all connected clients
	 * 
	 * Sends the same message to all currently connected clients.
	 * Handles dead client cleanup by removing clients that fail to receive.
	 * 
	 * @param msgBuf Message buffer to broadcast
	 * @param msgLen Length of the message
	 */
	void broadcast(const char* msgBuf, int msgLen);
	
	/**
	 * @brief Sends data with protocol header to a client
	 * 
	 * Sends data to a client with the proper protocol header.
	 * Ensures all data is sent (handles partial sends).
	 * 
	 * @param s Client socket
	 * @param buf Data buffer to send
	 * @param len Length of data
	 * @return true if send successful, false otherwise
	 */
	bool sendAll(SOCKET s, const char* buf, int len);
	
	/**
	 * @brief Removes a dead client from server structures
	 * 
	 * Cleans up client data by removing from client vector and username map.
	 * Called when a client disconnects or communication fails.
	 * 
	 * @param s Socket of the dead client
	 */
	void removeDeadClient(SOCKET s);

	
};
}

