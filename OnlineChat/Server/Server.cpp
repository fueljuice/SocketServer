#include "Server.h"


#define dbFileDir   "C:\\Users\\zohar\\Desktop\\dbFile.txt"
#define INTSIZE     4

// constructor that init the base server
sockets::server::Server::Server(int domain, int service, int protocol,
    int port, u_long network_interaface, int backlog)
    : ServerInterface(domain, service, protocol,
        port, network_interaface, backlog)
{
    std::cout << "init Server" << std::endl;
    // opens a handle to the file which is used as the database.
    dbFile.open(dbFileDir, std::ios::in | std::ios::out | std::ios::app);
}

void sockets::server::Server::launch()
{
    lstnSocket->startLisetning();
    // changing the atomic member that represents the server running, to true 
   running.store(true);
   acceptConnection();
}

// force shuts the server
void sockets::server::Server::stop()
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


sockets::server::Server::~Server()
{
    // to avoid leaks, closing the file handle and stopping the serevr on destruction
    stop();
    if (dbFile.is_open())
        dbFile.close();
}



void sockets::server::Server::acceptConnection()
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
void sockets::server::Server::onClientAccept(std::shared_ptr<data::ClientSocketData> client)
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
    std::cout << "recving length" << std::endl;
    // rhandling the client until a force stop or a closed client socket
    while(running.load())
    {
        // recving the entire header. which is 8 bytes (2 * INTSIZE)
        lengthHeaderBytes = recv(
            client->clientSocket,
            client->dataBuf.get(),
            2 * INTSIZE,
            MSG_WAITALL
        );

        printf("buf: %8s\n", client->dataBuf.get());

        
        if (lengthHeaderBytes != 2 * INTSIZE)
        {
            std::cout << "couldnt read length from client. bytes read: " << lengthHeaderBytes << std::endl;
            std::cout << "last problem: " << WSAGetLastError() << std::endl;
            return;
        }
        std::cout << "calling pp " << std::endl;

        //  parses the header
        messaging::ParsingProtocol pp(client->dataBuf.get(), 2 * INTSIZE);
        messaging::ParsedRequest pr = pp.parseHeader(); // the length of the request
        std::cout << "length: : " << pr.dataSize << std::endl;

        // if the request is getchat, theres no need in recving again
        if(pr.requestType == messaging::GETCHAT)
            respondToClient(client, pr);


        else
        {
            // waiting until the entire meessage arrives using the length 
            // we got and the MSG_WAITALL flag
            std::cout << "recving from client" << std::endl;
            bodyBytes = recv(
                client->clientSocket,
                client->dataBuf.get(),
                pr.dataSize, // the recv expects the body itself
                MSG_WAITALL
            );
            std::cout << "done rcev" << std::endl;

            if (bodyBytes > 0)
            {
                // sucsessful read. storing data and responding.
                printf("Bytes received: %d\n", bodyBytes);
                printf("recv data: %s\n", client->dataBuf.get());
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

}

// prases the data and detremines which request to serve
void sockets::server::Server::respondToClient(std::shared_ptr<data::ClientSocketData> client, messaging::ParsedRequest& pr)
{

    std::cout << "responding to client..." << std::endl;
    // parsing with the previous header as parameter 
    messaging::ParsingProtocol pp(std::move(pr), client.get()->dataBuf.get(), client.get()->lenData);

    // getting the new parsed request with data
    messaging::ParsedRequest refinedPr = pp.parseData();
    
    if (refinedPr.statusCode != 200)
    {
        std::cout << "STATUS CODE BAD 404" << std::endl;
    }
    else
    {
        // in a case of valid request match the request to the function
        std::cout << "STATUS CODE OK 200" << std::endl;

        // matching which request type was asked for 
        switch (refinedPr.requestType)
        {

            // sendmessage request
            case messaging::SENDMESSAGE:
            {
                std::cout << "send message request" << std::endl;
                sendMessage(client, refinedPr);
                break;
            }

            // get chat request
            case messaging::GETCHAT:
            {
                std::cout << "get chat request" << std::endl;
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
void sockets::server::Server::getChat(std::shared_ptr<data::ClientSocketData> client, messaging::ParsedRequest& pr)
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
        // iterating over all the file to get all the text from it
        allText = std::string
        {
            std::istreambuf_iterator<char>(dbFile),
            std::istreambuf_iterator<char>()
        };

        // if the text passing 4 bytes, it's size wont fit in the header ( 4 bytes )
        // therfore its got to be cut
        if (allText.size() > 9999)
        {
            std::cout << "text is too big.. CUTTING IT TO SIZE 9999" << std::endl;
            allText.resize(9999);

        }

        if (!dbFile.good() && !dbFile.eof())
        {
            std::cout << "can't read file" << std::endl;
            return;
        }
    }

    {
        std::cout << "allText:" << allText << "\nsize of allText: " << allText.size() << std::endl;
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
        std::cout << "client socket unavaiable. cant send client" << std::endl;
        // Move the lock_guard outside the erase call to ensure the lock is held during erase
        {
            removeDeadClient(client->clientSocket);
        }
    }

}

// reads a message from the client to the data base file. and brodcasts the update to all active users
void sockets::server::Server::sendMessage(std::shared_ptr<data::ClientSocketData> client, messaging::ParsedRequest& pr)
{

    std::cout << "sendMessage request called" << std::endl;
    {
        std::lock_guard<std::mutex> lk(fileMutex);
        dbFile.clear();
        dbFile.seekp(0, std::ios::end);
        dbFile << pr.databuffer << std::endl;
        dbFile.flush();
    }
    broadcast(pr.databuffer, pr.dataSize);

}

// broadcasts message to all active clients 
void sockets::server::Server::broadcast(const char* msgBuf, int msgLen)
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
// it also sends the first INTSIZE bytes as the length
bool sockets::server::Server::sendAll(SOCKET s, const char* buf, int len)
{
    int sent = 0, r, sentLength;
    char formattedLength[INTSIZE + 1] = {0};

    // sending the first INTSIZE bytes as the length of the message
    sprintf_s(formattedLength, sizeof(formattedLength), "%0*d", INTSIZE, len); // formatting
    sentLength = send(s, formattedLength, INTSIZE, 0);
    if (sentLength != INTSIZE)
        return false;

    

    // sending the buffer itself
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