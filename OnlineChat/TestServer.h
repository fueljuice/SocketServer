#pragma once
#include "SocketServer.h"
#include "ParsingProtocol.h"
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <string>
#include <fstream>

//TODO: need to contsntily check if new messages arrived.
// if it does send them to all current connections



namespace HDE
{

class TestServer : public SocketServer
{
private:
	std::atomic_bool running{ false };
	std::thread threadPerClient;
	std::fstream dbFile;
	std::mutex sendMutex;
	std::mutex clientVectorMutex;
	std::mutex fileMutex;
	std::vector< std::shared_ptr<ClientSocketData>> clientVector;


	void acceptConnection() override;

	void handleConnection(ClientSocketData client) override;

	void onClientAccept(ClientSocketData& client) override;

	void broadcast(const char* msgBuf, int msgLen) override;

	void respondToClient(ClientSocketData& client, int readLength) override;

	void handleClientData(ClientSocketData& client, int readLength);

	void getChat(ClientSocketData& client, messaging::ParsedRequest pr);

	void sendMessage(ClientSocketData& client, messaging::ParsedRequest pr);

	bool sendAll(int s, const char* buf, int len);


public:
	void launch();
	
	~TestServer();

	void stop() override;

	TestServer(int domain, int service, int protocol,
		int port, u_long network_interaface, int backlog);
	
};
}

