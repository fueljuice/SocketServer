#include "Server.h"
#include "ChatRequestHandler.h"
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
    netIO(std::make_unique<NetworkIO>()),
	registry(std::make_unique<UserRegistry>()),
	dbManager(std::make_unique<DataBaseManager>()),
	sessionManager(std::make_unique<SessionManager>()),
	requestHandler(std::make_unique<ChatRequestHandler>(netIO, registry, dbManager, sessionManager))

{
    // opens a handle to the file which is used as the database.
    dbManager->dbInit();
    DBG("init Server.");
}

void sockets::server::Server::launch()
{
   lstnSocket->startLisetning();
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
	sessionManager->endSession();
    // joins every current running threads
    for (auto& cThread : clientThreads)
    {
        if (cThread.joinable())
            cThread.join();
    }
}


sockets::server::Server::~Server()
{
    // to avoid leaks, closing the file handle and stopping the serevr on destruction
    if(running.load())
        stop();
    dbManager->dbClose();
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
			sessionManager->addClient(newSock, clientAddr);
            clientThreads.emplace_back(
                &sockets::server::Server::handleConnection,
                this,
                newSock);
        }
    }
}


// handles client by recviing the length of the clients data. then reading from its socket the length amount.
void sockets::server::Server::handleConnection(SOCKET sock)
{
    while(running.load())
    {
        // recving data
        std::string header = netIO->recvAll(sock, messaging::REQUEST_HEADER_SIZE);

        // update metadata
		sessionManager->setClientHeader(sock, header);

        // parses the header
        auto parsedRqst = messaging::ServerProtocol::parseHeader(header.data(), header.size());
        if (!parsedRqst)
        {
            DBG("HEADER ERROR");
            removeDeadClient(sock);
            return;
        }

        // skip reading if there is no data
        if (parsedRqst->dataSize == 0)
        {
            respondToClient(sock, parsedRqst.value());
            continue;
        }
        
		// recving the body of the request
        DBG("recving data from client");
        std::string body = netIO->recvAll(sock, parsedRqst->dataSize);

        
        // sucsessful read. storing data and responding.
        DBG("done rcev data");
        if (body.size() <= 0)
        {
            DBG("recv failed");
            removeDeadClient(sock);
            return;
        }
		sessionManager->setClientData(sock, body);
        respondToClient(sock, parsedRqst.value());
    }

}

// prases the data and detremines which request to serve
void sockets::server::Server::respondToClient(SOCKET sock, messaging::ParsedRequest& oldParsedRqst)
{
    auto refinedPr = 
        messaging::ServerProtocol::parseData(std::move(oldParsedRqst),
        sessionManager->getClientData(sock).data());

    if (messaging::ServerProtocol::isStatusOK(refinedPr, registry->isClientExist(sock)))
    {
        DBG("STATUS CODE OK 200");
        requestHandler->handleRequest(sock, refinedPr);
        return;
    }

    DBG("STATUS CODE BAD 404");	

}

void sockets::server::Server::removeDeadClient(SOCKET s)
{
    // remove session and from registry
	sessionManager->removeClient(s);
    registry->eraseClient(s);
}