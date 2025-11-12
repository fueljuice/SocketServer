#include "TestServer.h"
#define dbFileDir   "C:\\Users\\zohar\\Desktop\\dbFile.txt"


// constructor that init the base server
sockets::server::TestServer::TestServer(int domain, int service, int protocol,
    int port, u_long network_interaface, int backlog)
    : SocketServer(domain, service, protocol,
        port, network_interaface, backlog)
{
    std::cout << "init TestServer" << std::endl;
    // opens a handle to the file which is used as the database.
    dbFile.open(dbFileDir, std::ios::in | std::ios::out | std::ios::app);
}

void sockets::server::TestServer::launch()
{
    lstnSocket->startLisetning();
    // changing the atomic member that represents the server running, to true 
   running.store(true);
   acceptConnection();
}

// force shuts the server
void sockets::server::TestServer::stop()
{

    std::cout << "stopped server" << std::endl;
    // stopping the atomic running member
    running.store(false);

    // closes the socket listener 
    lstnSocket->stopLisetning();


    {
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


sockets::server::TestServer::~TestServer()
{
    // to avoid leaks, closing the file handle and stopping the serevr on destruction
    stop();
    if (dbFile.is_open())
        dbFile.close();
}



void sockets::server::TestServer::acceptConnection()
{

    socklen_t addrLen;
    SOCKET newSock;
    std::cout << "accepting..." << std::endl;

    // running while the atomic member is true (on).
    while(running.load())
    {
        sockaddr clientAddr{};
        addrLen = sizeof(clientAddr);

        // async function that waits for a client
        newSock = lstnSocket->acceptCon(
            reinterpret_cast<sockaddr*>(&clientAddr),
            &addrLen);

        std::cout << "done accept..." << std::endl;

        // in the case of a legit socket
        if (newSock != INVALID_SOCKET)
        {
            std::cout << "accepted valid socket" << std::endl;
            auto clientPtr = std::make_shared<data::ClientSocketData>(newSock, clientAddr, 5000);
            {
                // locking before pushing the socket of the client into the list of clients.
                // the list is later used to brod
                std::lock_guard<std::mutex> lk(clientVectorMutex);  
                clientVector.push_back(clientPtr);
            }
            onClientAccept(clientPtr);
        
        }
        else
        {
            std::cout << "accpeted invalid socket" << std::endl;
        }
    }

}

// opens a new thread for each client accepted
void sockets::server::TestServer::onClientAccept(std::shared_ptr<data::ClientSocketData> client)
{
    // keeps the client threads in a vector, to later join them easily in a loop.
    clientThreads.emplace_back(
        &sockets::server::TestServer::handleConnection,
        this,
        std::move(client));

}


// handles client by recviing the length of the clients data. then reading from its socket the length amount.
void sockets::server::TestServer::handleConnection(std::shared_ptr<data::ClientSocketData> client)
{
    int bodyBytes, lengthHeaderBytes, totalrecv = 0;

    // rhandling the client until a force stop or a closed client socket
    while(running.load())
    {
        // receving only the header that contains the length of the body
        lengthHeaderBytes = recv(
            client->clientSocket,
            client->dataBuf.get(),
            4,
            MSG_WAITALL
        );

        printf("buf: %4s\n", client->dataBuf.get());

        
        if (lengthHeaderBytes != 4)
        {
            std::cout << "couldnt read length from client. bytes read: " << lengthHeaderBytes << std::endl;
            std::cout << "last problem: " << WSAGetLastError() << std::endl;
            return;
        }
        std::cout << "calling pp " << std::endl;

        //  parses the length of the body
        messaging::ParsingProtocol pp(client->dataBuf.get(), 4);
        messaging::ParsedRequest pr = pp.enforceProtocol(); // the length of the request
        std::cout << "length: : " << pr.dataSize << std::endl;

        // waiting until the entire meessage arrives using the length 
        // we got and the MSG_WAITALL flag
        std::cout << "recving from client" << std::endl;
        bodyBytes = recv(
            client->clientSocket,
            client->dataBuf.get(),
            pr.dataSize,
            MSG_WAITALL
        );
        std::cout << "done rcev" << std::endl;

        if (bodyBytes > 0)
        {
            // sucsessful read. storing data and responding.
            printf("Bytes received: %d\n", bodyBytes);
            printf("recv data: %4s\n", client->dataBuf.get());
            client.get()->lenData = bodyBytes;
            respondToClient(client, pr);
        }

        else if (bodyBytes == 0)
        {
            printf("Connection closed recv 0\n");
            removeDeadClient(client->clientSocket);
            break;
        }

        else
        {
            printf("recv failed: %d\n", WSAGetLastError());
            removeDeadClient(client->clientSocket);
            break;
        }
    }

}

// prases the data and detremines which request to serve
void sockets::server::TestServer::respondToClient(std::shared_ptr<data::ClientSocketData> client, messaging::ParsedRequest& pr)
{

    std::cout << "responding to client..." << std::endl;
    // parsing request
    messaging::ParsingProtocol pp(pr, client.get()->dataBuf.get(), client.get()->lenData);
    messaging::ParsedRequest refinedPr = pp.enforceProtocol();

    if (refinedPr.statusCode != 200)
    {
        std::cout << "STATUS CODE BAD 404" << std::endl;
    }
    else
    {
        // in a case of valid request match the request to the function
        std::cout << "STATUS CODE OK 200" << std::endl;
        switch (refinedPr.requestType)
        {

            case messaging::SENDMESSAGE:
            {
                std::cout << "" << std::endl;
                sendMessage(client, refinedPr);
                break;
            }

            case messaging::GETCHAT:
            {
                getChat(client, refinedPr);
                break;
            }

            default:
            {
                std::cout << "BAD REQUEST TYPE" << std::endl;
                break;
            }

        }
    }
}

// sends the entire data base to the client, according to the protocol
void sockets::server::TestServer::getChat(std::shared_ptr<data::ClientSocketData> client, messaging::ParsedRequest& pr)
{
    std::cout << "getChat request called" << std::endl;
    bool isSent;
    std::string allText;

    {
        // locking the file with mutex to prevent a thread updating the file while reading

        std::lock_guard<std::mutex> lk(fileMutex);
        if (!dbFile.is_open())
        {
            std::cout << "cant open file" << std::endl;
            return;
        }

        dbFile.clear();
        dbFile.seekg(0, std::ios::beg);
        allText = std::string
        {
            std::istreambuf_iterator<char>(dbFile),
            std::istreambuf_iterator<char>()
        };

        if (!dbFile.good() && !dbFile.eof())
        {
            std::cout << "can't read file" << std::endl;
            return;
        }
    }

    {
        std::lock_guard<std::mutex> lk(sendMutex);
        isSent = sendAll(
            client->clientSocket,
            allText.c_str(),
            static_cast<int>(allText.size())
        );
    }

    if (!isSent)
    {
        std::cout << "client socket unavaiable. cant send client" << std::endl;
        // Move the lock_guard outside the erase call to ensure the lock is held during erase
        {
            removeDeadClient(client->clientSocket);
        }
    }

}

// reads a message from the client to the data base file. and brodcasts the update to all active users
void sockets::server::TestServer::sendMessage(std::shared_ptr<data::ClientSocketData> client, messaging::ParsedRequest& pr)
{

    std::cout << "sendMessage request called" << std::endl;
    {
        std::lock_guard<std::mutex> lk(fileMutex);
        dbFile.clear();
        dbFile.seekp(0, std::ios::end);
        dbFile << pr.databuffer << std::endl;
        dbFile.flush();
    }
    broadcast(pr.databuffer, static_cast<int>(std::strlen(pr.databuffer)));

}


// broadcasts message to all active clients 
void sockets::server::TestServer::broadcast(const char* msgBuf, int msgLen)
{
    bool isSent;
    std::vector<SOCKET> deadClients;
    {
        std::lock_guard<std::mutex> lk(clientVectorMutex);
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

            if (!isSent)
            {
                deadClients.push_back(client.get()->clientSocket);
            }
        }
    }


    // removing by socket all the dead clients ( with unavaiable sockets)
    for (SOCKET deadClientSocket : deadClients)
    {
        removeDeadClient(deadClientSocket);
    }

}

// function the makes sure every part of the buffer is sent, given the length
bool sockets::server::TestServer::sendAll(SOCKET s, const char* buf, int len)
{
    int sent = 0;
    while (sent < len)
    {
        int r = send(s, buf + sent, len - sent, 0);
        if (r <= 0) return false;
        sent += r;
    }
    return true;
}

// deletes a client from the client vector of all active clients
void sockets::server::TestServer::removeDeadClient(SOCKET s) 
{
    std::lock_guard<std::mutex> lk(clientVectorMutex);
    std::erase_if(
        clientVector,

        [s](const std::shared_ptr<data::ClientSocketData>& p)
        {
        if (p && p->clientSocket == s) // lambda that checks if the client struct holds the socket s
        {
            return true;
        }
        return false;
    });
}