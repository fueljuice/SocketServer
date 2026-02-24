#include "Server.h"
#include "../Protocol/ProtocolConstants.h"

#define dbFileDir   "C:\\Users\\zohar\\Desktop\\dbFile.txt"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

// constructor that init the base server
sockets::server::Server::Server(int domain, int service, int protocol,
    int port, u_long network_interaface, int backlog)
    : AbstractServer(domain, service, protocol,
        port, network_interaface, backlog)
{
    DBG("init Server.");
    // opens a handle to the file which is used as the database.
    dbFile.open(dbFileDir, std::ios::in | std::ios::out | std::ios::app);
}

void sockets::server::Server::launch()
{
   lstnSocket->startLisetning();
   running.store(true);
   acceptConnection();
}

// force shuts the server
void sockets::server::Server::stop()
{
    DBG("stopped server");
    // stopping the atomic running member
    running.store(false);

    // closes the socket listener 
    lstnSocket->stopLisetning();
    {
        // clearing each socket cached
        std::lock_guard<std::mutex> lk(clientVectorMutex);
        for (auto& c : clientVector)
        {
            // shuts down every client socket, to avoid infintily waiting to
            // recv() with the WAITALL flag
            shutdown(c->clientSocket, SD_BOTH);
            closesocket(c->clientSocket);
        }
    }

    // joins every current running threads
    for (auto& cThread : clientThreads)
    {
        if (cThread.joinable())
            cThread.join();
    }

    {
        // clears the clients vector and  destroys the last ownership if the shared pointer to data::ClientSocketData struct
        std::lock_guard<std::mutex> lk(clientVectorMutex);
        clientVector.clear();
    }
    clientThreads.clear();
}


sockets::server::Server::~Server()
{
    // to avoid leaks, closing the file handle and stopping the serevr on destruction
    if(running.load())
        stop();
    if (dbFile.is_open())
        dbFile.close();
}


void sockets::server::Server::acceptConnection()
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
            auto clientPtr = std::make_shared<data::ClientSocketData>(newSock, clientAddr, messaging::MAX_CLIENT_BYTES);
            {
                // locking before pushing the socket of the client into the list of clients.
                // the list is later used to brod
                std::lock_guard<std::mutex> lk(clientVectorMutex);  
                clientVector.push_back(clientPtr);
            }
            openThreadForClient(clientPtr);
        }
    }
}

// opens a new thread for each client accepted
void sockets::server::Server::openThreadForClient(std::shared_ptr<data::ClientSocketData> client)
{
    // keeps the client threads in a vector, to later join them easily in a loop.
    clientThreads.emplace_back(
        &sockets::server::Server::handleConnection,
        this,
        std::move(client));
}

// handles client by recviing the length of the clients data. then reading from its socket the length amount.
void sockets::server::Server::handleConnection(std::shared_ptr<data::ClientSocketData> client)
{
    int bodyBytes, lengthHeaderBytes, totalrecv = 0;

    while(running.load())
    {
        // recving the entire header
        std::array<std::byte, messaging::REQUEST_HEADER_SIZE> header{};
        lengthHeaderBytes = recv(
            client->clientSocket,
            reinterpret_cast<char*>(header.data()),
            static_cast<int>(header.size()),
            MSG_WAITALL
        );

        // parses the header, check if header is OK
        messaging::ParsedRequest pr = 
            messaging::ServerProtocol::parseHeader(
                reinterpret_cast<const char*>(header.data())
                , lengthHeaderBytes);
        if (!messaging::ServerProtocol::isHeaderOK(pr))      
        {
            DBG("HEADER ERROR");
            removeDeadClient(client->clientSocket);
            return;
        }

        // skip reading if there is no data
        if (pr.dataSize == 0)
        {
            client->lenData = 0;
            respondToClient(client, pr); 
            continue;
        }
        
        // waiting until the entire meessage arrives using the length 
        // MSG_WAITALL flag to wait for all the message to arrive
        DBG("recving data from client");
        bodyBytes = recv(
            client->clientSocket,
            client->dataBuf.get(),
            pr.dataSize, // the recv expects the body itself
            MSG_WAITALL
        );

        DBG("done rcev data");
        // sucsessful read. storing data and responding.
        if (bodyBytes < 0)
        {
            DBG("recv failed");
            removeDeadClient(client->clientSocket);
            return;
        }
        client.get()->lenData = bodyBytes;
        respondToClient(client, pr);
   
    }

}

// prases the data and detremines which request to serve
void sockets::server::Server::respondToClient(std::shared_ptr<data::ClientSocketData> client, messaging::ParsedRequest& oldPr)
{

    DBG("responding to client...");
    // parsing with the previous header as parameter 
    // getting the new parsed request with data
    messaging::ParsedRequest refinedPr = 
        messaging::ServerProtocol::parseData(std::move(oldPr), client->dataBuf.get());
    if (!messaging::ServerProtocol::isStatusOK(refinedPr))
    {
        DBG("STATUS CODE BAD 404");
        return;
    }

    DBG("STATUS CODE OK 200");
    // matching which request type was asked for 
    switch (refinedPr.requestType)
    {

        // sendmessage request
    case messaging::ActionType::SEND_MESSAGE:
        {
            DBG("send message request");
            sendMessage(client, refinedPr);
            break;
        }

        // get chat request
    case messaging::ActionType::GET_CHAT:
    {
            DBG("get chat request");
            getChat(client, refinedPr);
            break;
    }

    case messaging::ActionType::REGISTER:
        {
        DBG("register request");
            registerRequest(client, refinedPr);
            break;
        }

    default:
        {
        DBG("BAD REQUEST TYPE");
        break;
        }
  
    }
}

// sends the entire data base to the client, according to the protocol
void sockets::server::Server::getChat(std::shared_ptr<data::ClientSocketData> client, messaging::ParsedRequest& pr)
{
    DBG("getChat request called");
    bool isSent;
    std::string allText;

    {
        // locking the file with mutex to prevent a thread updating the file while reading

        std::lock_guard<std::mutex> lk(fileMutex);
        if (!dbFile.is_open())
        {
            DBG("cant open file");
            stop();
        }

        dbFile.clear();
        dbFile.seekg(0, std::ios::beg);
        // iterating over all the file to get all the text from it
        allText = std::string
        {
            std::istreambuf_iterator<char>(dbFile),
            std::istreambuf_iterator<char>()
        };

        // if the text passing 4 bytes, it's size wont fit in the header ( 4 bytes )
        // therfore its got to be cut
        if (allText.size() > messaging::MAX_MESSAGE_LENGTH)
        {
            DBG("text is too big.. CUTTING IT TO SIZE " << messaging::MAX_MESSAGE_LENGTH);
            allText.resize(messaging::MAX_MESSAGE_LENGTH);

        }

        if (!dbFile.good() && !dbFile.eof())
        {
            DBG("can't read file");
            return;
        }
    }

    {
        DBG("allText:" << allText << "\nsize of allText: " << allText.size());
        // sending the file text to the client
        std::lock_guard<std::mutex> lk(sendMutex);
        isSent = sendAll(
            client->clientSocket,
            allText.c_str(),
            static_cast<int>(allText.size())
        );
    }

    if (!isSent)
    {
        DBG("client socket unavaiable. cant send client");
        // Move the lock_guard outside the erase call to ensure the lock is held during erase
        {
            removeDeadClient(client->clientSocket);
        }
    }

}

// reads a message from the client to the data base file. and brodcasts the update to all active users
void sockets::server::Server::sendMessage(std::shared_ptr<data::ClientSocketData> client, messaging::ParsedRequest& pr)
{
    // checking if the client is registered
    if(clientsNameMap[client->clientSocket].empty())
    {
		const char* msg = "Please register before sending messages.";
        sendAll(client->clientSocket, "Please register first", 22);(
            client->clientSocket,
            msg,
            static_cast<int>(strlen(msg))
			);
        return;
	}

    // writing the message to the file
    DBG("sendMessage request called");
    {
        std::lock_guard<std::mutex> lk(fileMutex);
        if (!dbFile.is_open())
        {
            DBG("cant open file");
            stop();
        }
        dbFile.clear();
        dbFile.seekp(0, std::ios::end);
        dbFile << clientsNameMap[client->clientSocket] << ": " << pr.dataBuffer << std::endl;
        dbFile.flush();
    }
    // broadcasting the message
    broadcast(pr.dataBuffer.c_str(), static_cast<int>(pr.dataBuffer.length()));

}

void sockets::server::Server::registerRequest(std::shared_ptr<data::ClientSocketData> client, messaging::ParsedRequest& pr)
{
    // checking if the client is already registered
    if(!clientsNameMap[client->clientSocket].empty())
    {
        const char* msg = "You are already registered.";
        sendAll(client->clientSocket, msg, static_cast<int>(strlen(msg)));
        return;
	}
    DBG("start regitering");
    
    // check for duplicate usernames
    std::string requestedUsername = pr.dataBuffer;
    for (const auto& [socket, username] : clientsNameMap)
    {
        if (username == requestedUsername)
        {
            const char* msg = "Username already taken. Please choose a different username.";
            sendAll(client->clientSocket, msg, static_cast<int>(strlen(msg)));
            return;
        }
    }

    clientsNameMap[client->clientSocket] = requestedUsername;

    // send success message
    const char* successMsg = "Registration successful.";
    sendAll(client->clientSocket, successMsg, static_cast<int>(strlen(successMsg)));
}

// broadcasts message to all active clients 
void sockets::server::Server::broadcast(const char* msgBuf, int msgLen)
{
    DBG("broadcasting...");
    bool isSent;
    std::vector<SOCKET> deadClients;
    {
        std::lock_guard<std::mutex> lk(clientVectorMutex);
        // iterating over all the active clients
        for (const auto& client : clientVector)
        {
            if (!running.load())
                break;

            {
                std::lock_guard<std::mutex> lk(sendMutex);
                isSent = sendAll(
                    client.get()->clientSocket,
                    msgBuf,
                    msgLen
                );
            }
			// if the client is unavaiable, adding it to the dead clients vector to remove it later
            if (!isSent)
            {
                deadClients.push_back(client.get()->clientSocket);
            }
        }
        DBG("ended broadcast ");
    }


    // removing by socket all the dead clients ( with unavaiable sockets)
    for (SOCKET deadClientSocket : deadClients)
    {
        removeDeadClient(deadClientSocket);
    }

}

// function the makes sure every part of the buffer is sent, given the length
// it also sends the first INTSIZE bytes as the length
bool sockets::server::Server::sendAll(SOCKET s, const char* buf, int len)
{
    int sent = 0, r, sentLength;
    DBG("sending data...");
    // sends a response header
    sentLength = send(
        s,
        messaging::ServerProtocol::constructResponseHeader(len).c_str(),
        messaging::REQUEST_DATA_LENGTH_SIZE,
        0
    );
    DBG("sent header" << sentLength);
    if (sentLength != messaging::REQUEST_DATA_LENGTH_SIZE)
        return false;
    std::cout << buf << std::endl;
    // sending the data
    while (sent < len)
    {
        r = send(s, buf + sent, len - sent, 0);
        if (r <= 0) 
            return false;
        sent += r;
    }
    return true;
}


// deletes a client from the client vector of all active clients
void sockets::server::Server::removeDeadClient(SOCKET s) 
{
    DBG("removing dead client");
    std::lock_guard<std::mutex> lk(clientVectorMutex);
    // remove client from vector
    std::erase_if(
        clientVector,

        [s](const std::shared_ptr<data::ClientSocketData>& p)
        {
        if (p && p->clientSocket == s) 
        {
            return true;
        }
        return false;
    });
	// remove client from name map
	clientsNameMap.erase(s);
}