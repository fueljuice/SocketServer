#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <utility>
#include <optional>
#include <atomic>
#include <memory>

#include "./data/clientSocketData.h"
#include "AbstractServer.h"
#include "../Protocol/ServerProtocol.h"
#include "./UserRegistry.h"
#include "./NetworkIO.h"
#include "./DataBaseManager.h"
#include "DataBaseManager.h"
#include "./SessionManager.h"
#include "./RequestHandler.h"
#include "./ClientConnectionWorker.h"
#include "./ClientThreadManager.h"
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

	std::unique_ptr<NetworkIO> net;
	std::unique_ptr<UserRegistry> registry;
	std::unique_ptr<DataBaseManager> database;
	std::unique_ptr<SessionManager> sessions;
	std::unique_ptr<RequestHandler> handler;
	std::unique_ptr<ClientConnectionWorker> worker;
	std::unique_ptr<ClientThreadManager> clientThreads;

	void acceptConnections();

	void handleConnection(SOCKET sock);

	void removeDeadClient(SOCKET s);

	
};
}

