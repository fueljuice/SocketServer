#pragma once

#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <fstream>

#include "./data/clientSocketData.h"
#include "AbstractServer.h"
#include "../Parser/ParsingProtocol.h"



namespace sockets::server
{

class Server final: public AbstractServer
{

public:
	void launch() override;
	void stop() override;
	Server(int domain, int service, int protocol,
		int port, u_long network_interaface, int backlog);
	~Server();

private:
	// atomic bool that can stop threads ina case of stopping the server
	std::atomic_bool running{ false };
	// file handle
	std::fstream dbFile;
	// mutex to avoid sending to the same client in the same time.
	std::mutex sendMutex;
	// mutex to avoid race on deleteing / adding clients at the same time
	std::mutex clientVectorMutex;
	// mutex to avoid opening multiple file handles
	std::mutex fileMutex;
	// keeps a thread vector for every client so can close them anytime
	std::vector<std::thread> clientThreads;
	// vector of shared clients to iterate over for broadcast. its shared in order let both the API and the
	// the vector itself to own and close it
	std::vector<std::shared_ptr<data::ClientSocketData>> clientVector;


	// accpeting clients
	void acceptConnection();
	void onClientAccept(std::shared_ptr<data::ClientSocketData> client);

	// handling the client
	void handleConnection(std::shared_ptr<data::ClientSocketData> client);
	void respondToClient(std::shared_ptr<data::ClientSocketData> client, messaging::ParsedRequest& pr);

	// serving GETCHAT request. sending the entire chat log to client.
	void getChat(std::shared_ptr<data::ClientSocketData> client, messaging::ParsedRequest& pr);

	// serving sendMessage request. logging a message into the database and sending it to all connected users
	void sendMessage(std::shared_ptr<data::ClientSocketData> client, messaging::ParsedRequest& pr);

	// util functions
	void broadcast(const char* msgBuf, int msgLen);
	bool sendAll(SOCKET s, const char* buf, int len);
	void removeDeadClient(SOCKET s);

	
};
}

