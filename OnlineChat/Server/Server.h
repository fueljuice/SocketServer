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
#include "../Protocol/ProtocolConstants.h"
#include "ClientThreadManager.h"
#include "ServerException.h"
#include "NetworkIO.h"
#include "UserRegistry.h"
#include "DataBaseManager.h"
#include "SessionManager.h"
#include "RequestHandler.h"
#include "ClientConnectionWorker.h"
namespace sockets::server
{

class Server final: public AbstractServer
{

public:
	void launch() override;
	void stop() override;
	

	Server(int domain, int service, int protocol,
		int port, u_long network_interaface, int backlog);
	Server(int domain, int service, int protocol,
		int port, u_long network_interaface, int backlog,
		std::unique_ptr<INetworkIO> net,
		std::unique_ptr<IUserRegistry> registry,
		std::unique_ptr<IdbManager> database,
		std::unique_ptr<ISessionManager> sessions,
		std::unique_ptr<IRequestHandler> handler,
		std::unique_ptr<IClientConnectionWorker> worker,
		std::unique_ptr<ClientThreadManager> clientThreads);
	~Server();

private:
	void initWorker(SOCKET sock);
	std::atomic_bool running;

	std::unique_ptr<INetworkIO> net;
	std::unique_ptr<IUserRegistry> registry;
	std::unique_ptr<IdbManager> database;
	std::unique_ptr<ISessionManager> sessions;
	std::unique_ptr<IRequestHandler> handler;
	std::unique_ptr<IClientConnectionWorker> worker;
	std::unique_ptr<ClientThreadManager> clientThreads;

	void acceptConnections();
	
};
}

