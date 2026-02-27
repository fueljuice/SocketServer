#include "Server.h"
#include "RequestHandler.h"
#include "../Protocol/ProtocolConstants.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

// constructor that init the base server
sockets::server::Server::Server(int domain, int service, int protocol,
    int port, u_long network_interaface, int backlog)
    : 
    AbstractServer(domain, service, protocol, port, network_interaface, backlog),
    net(std::make_unique<NetworkIO>()),
	registry(std::make_unique<UserRegistry>()),
	database(std::make_unique<DataBaseManager>()),
	sessions(std::make_unique<SessionManager>()),
    clientThreads(std::make_unique<ClientThreadManager>()),
	handler(std::make_unique<RequestHandler>(*net, *registry, *database, *sessions)),
	worker(std::make_unique<ClientConnectionWorker>(*net, *sessions, *registry, *handler))

{
    // opens a handle to the file which is used as the database.
    database->dbInit();
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
			clientThreads->start(&Server::handleConnection, this, std::move(newSock));
        }
    }
}


void sockets::server::Server::handleConnection(SOCKET sock)
{
    worker->run(sock);
}

void sockets::server::Server::removeDeadClient(SOCKET s)
{
    // remove session and from registry
	sessions->removeClient(s);
    registry->eraseClient(s);
}