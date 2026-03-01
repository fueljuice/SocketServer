#include "Server.h"
#include "RequestHandler.h"
#include "../Protocol/ProtocolConstants.h"

#include "ClientConnectionWorker.h"
#include "ClientThreadManager.h"
#include "DataBaseManager.h"
#include "NetworkIO.h"
#include "SessionManager.h"
#include "UserRegistry.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

// constructor that init the base server
sockets::server::Server::Server(int domain, int service, int protocol,
    int port, u_long network_interaface, int backlog)
    : Server(domain, service, protocol, port, network_interaface, backlog,
		std::make_unique<NetworkIO>(),
		std::make_unique<UserRegistry>(),
		std::make_unique<DataBaseManager>(),
		std::make_unique<SessionManager>(),
		nullptr,
		nullptr,
		std::make_unique<ClientThreadManager>())
{
	// default construction path keeps prior behavior
	handler = std::make_unique<RequestHandler>(*net, *registry, *database, *sessions);
	worker = std::make_unique<ClientConnectionWorker>(*net, *sessions, *registry, *handler);
	database->dbInit();
}

sockets::server::Server::Server(int domain, int service, int protocol,
	int port, u_long network_interaface, int backlog,
	std::unique_ptr<INetworkIO> net,
	std::unique_ptr<IUserRegistry> registry,
	std::unique_ptr<IdbManager> database,
	std::unique_ptr<ISessionManager> sessions,
	std::unique_ptr<IRequestHandler> handler,
	std::unique_ptr<IClientConnectionWorker> worker,
	std::unique_ptr<ClientThreadManager> clientThreads)
	:
	AbstractServer(domain, service, protocol, port, network_interaface, backlog),
	net(std::move(net)),
	registry(std::move(registry)),
	database(std::move(database)),
	sessions(std::move(sessions)),
	handler(std::move(handler)),
	worker(std::move(worker)),
	clientThreads(std::move(clientThreads)),
	running(false)

{
	DBG("init Server.");
}

void sockets::server::Server::launch()
{
   if (!lstnSocket->startLisetning())
   {
	   std::cout << "SERVER ALREADY RUNNING" << std::endl;
       return;
   }
   running.store(true);
   acceptConnections();
}

// force shuts the server
void sockets::server::Server::stop()
{
    DBG("stopped server");
    // stop the server
    running.store(false);

    // closes the socket listener 
	sessions->endSession();
    // joins every current running threads
    clientThreads->joinAll();
    // close handle to db
    database->dbClose();
}


sockets::server::Server::~Server()
{
    // to avoid leaks, closing the file handle and stopping the serevr on destruction
    if(running.load())
        stop();
}


void sockets::server::Server::acceptConnections()
{

    socklen_t addrLen;
    SOCKET newSock;
    DBG("accepting...");

    // running while the atomic member is true (on).
    while(running.load())
    {
        DBG("new accept loop");
        sockaddr clientAddr{};
        addrLen = sizeof(clientAddr);

        // async function that waits for a client
        newSock = lstnSocket->acceptCon(
            reinterpret_cast<sockaddr*>(&clientAddr),
            &addrLen);

        DBG("done acceping...");

        // in the case of a legit socket
        if (newSock != INVALID_SOCKET)
        {
            DBG("accepted valid socket");
			sessions->addClient(newSock, clientAddr);
            clientThreads->start(&sockets::server::Server::initWorker, this, std::move(newSock));
        }
    }
}

void sockets::server::Server::initWorker(SOCKET sock)
{
    worker->run(sock);
}