#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <utility>
#include <optional>
#include <atomic>

#include "./data/clientSocketData.h"
#include "AbstractServer.h"
#include "../Protocol/ServerProtocol.h"
#include "./UserRegistry.h"
#include "./NetworkIO.h"
#include "./DataBaseManager.h"
#include "DataBaseManager.h"
#include "./SessionManager.h"
#include "./ChatRequestHandler.h"
#include "./ServerException.h"
#include "../Protocol/ProtocolConstants.h"
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
	std::atomic_bool running{ false };
	std::vector<std::thread> clientThreads;

	std::unique_ptr<NetworkIO> netIO;
	std::unique_ptr<UserRegistry> registry;
	std::unique_ptr<DataBaseManager> dbManager;
	std::unique_ptr<SessionManager> sessionManager;
	std::unique_ptr<ChatRequestHandler> requestHandler;

	void acceptConnections();

	void handleConnection(SOCKET sock);

	void respondToClient(SOCKET sock, messaging::ParsedRequest& pr);

	void removeDeadClient(SOCKET s);

	
};
}

