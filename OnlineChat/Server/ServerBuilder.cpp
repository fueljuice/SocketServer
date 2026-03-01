#include "ServerBuilder.h"

#include <utility>

#include "Server.h"
#include "NetworkIO.h"
#include "UserRegistry.h"
#include "DataBaseManager.h"
#include "SessionManager.h"
#include "ClientThreadManager.h"
#include "RequestHandler.h"
#include "ClientConnectionWorker.h"

sockets::server::ServerBuilder::ServerBuilder(int domain, int service, int protocol,
	int port, u_long network_interaface, int backlog)
	: domain(domain)
	, service(service)
	, protocol(protocol)
	, port(port)
	, network_interaface(network_interaface)
	, backlog(backlog)
{
}

std::unique_ptr<sockets::server::Server> sockets::server::ServerBuilder::build()
{
	auto net = std::make_unique<NetworkIO>();
	auto registry = std::make_unique<UserRegistry>();
	auto database = std::make_unique<DataBaseManager>();
	auto sessions = std::make_unique<SessionManager>();
	auto clientThreads = std::make_unique<ClientThreadManager>();
	auto handler = std::make_unique<RequestHandler>(*net, *registry, *database, *sessions);
	auto worker = std::make_unique<ClientConnectionWorker>(*net, *sessions, *registry, *handler);

	database->dbInit();

	return std::make_unique<Server>(
		domain,
		service,
		protocol,
		port,
		network_interaface,
		backlog,
		std::move(net),
		std::move(registry),
		std::move(database),
		std::move(sessions),
		std::move(handler),
		std::move(worker),
		std::move(clientThreads));
}
