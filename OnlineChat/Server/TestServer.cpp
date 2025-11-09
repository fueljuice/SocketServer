#include "TestServer.h"
#define dbFileDir   "C:\\Users\\zohar\\Desktop\\dbFile.txt"



HDE::TestServer::TestServer(int domain, int service, int protocol,
    int port, u_long network_interaface, int backlog)
    : SocketServer(domain, service, protocol,
        port, network_interaface, backlog)
{
    std::cout << "init TestServer" << std::endl;
    // opens a handle to the file which is used as the database.
    dbFile.open(dbFileDir, std::ios::in | std::ios::out | std::ios::app);
}

void HDE::TestServer::launch()
{
    // changing the atomic member that represents the server running,  to true 
   running.store(true);
   acceptConnection();



}


void HDE::TestServer::stop()
{
    std::cout << "stopped server" << std::endl;
    // stopping the atomic running member
    running.store(false);
    lstnSocket->stopLisetning();
    for (auto& cThread : clientThreads)
    {
        if (cThread.joinable())
            cThread.join();
    }

}


HDE::TestServer::~TestServer()
{
    // to avoid leaks, closing the file handle and stopping the serevr on destruction
    stop();
    if (dbFile.is_open())
        dbFile.close();
}



void HDE::TestServer::acceptConnection()
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
            auto clientPtr = std::make_shared<ClientSocketData>(newSock, clientAddr, 5000);
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

void HDE::TestServer::onClientAccept(std::shared_ptr<ClientSocketData> client)
{
    clientThreads.emplace_back(
        &HDE::TestServer::handleConnection,
        this,
        std::move(client));

}



void HDE::TestServer::handleConnection(std::shared_ptr<ClientSocketData> client)
{
    int bodyBytes, lengthHeaderBytes, totalrecv = 0;
    std::cout << "is client up?: " << client->clientSocket << std::endl;
    bool isup = client->clientSocket == INVALID_SOCKET;
    std::cout << "is client socket invalid? : " << isup << std::endl;

    // getting the length of the request before iterating
    while(running.load())
    {
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

void HDE::TestServer::respondToClient(std::shared_ptr<ClientSocketData> client, messaging::ParsedRequest& pr)
{

    std::cout << "responding to client..." << std::endl;
    messaging::ParsingProtocol pp(pr, client.get()->dataBuf.get(), client.get()->lenData);
    messaging::ParsedRequest refinedPr = pp.enforceProtocol();

    if (refinedPr.statusCode == 404)
    {
        std::cout << "STATUS CODE BAD 404" << std::endl;
    }
    else
    {
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

void HDE::TestServer::getChat(std::shared_ptr<ClientSocketData> client, messaging::ParsedRequest& pr)
{
    std::cout << "getChat request called" << std::endl;
    bool isSent;
    std::lock_guard<std::mutex> lk(fileMutex);
    if (!dbFile.is_open()) 
    {
        std::cout << "cant open file" << std::endl;
        return;
    }

    dbFile.clear();
    dbFile.seekg(0, std::ios::beg);
    std::string allText = std::string{
        std::istreambuf_iterator<char>(dbFile),
        std::istreambuf_iterator<char>()
    };

    if (!dbFile.good() && !dbFile.eof())
    {
        std::cout << "can't read file" << std::endl;
        return;
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

void HDE::TestServer::sendMessage(std::shared_ptr<ClientSocketData> client, messaging::ParsedRequest& pr)
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



void HDE::TestServer::broadcast(const char* msgBuf, int msgLen)
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

bool HDE::TestServer::sendAll(SOCKET s, const char* buf, int len)
{
    int sent = 0;
    while (sent < len) {
        int r = send(s, buf + sent, len - sent, 0);
        if (r <= 0) return false;
        sent += r;
    }
    return true;
}

void HDE::TestServer::removeDeadClient(SOCKET s)
{
    std::lock_guard<std::mutex> lk(clientVectorMutex);
    // Find and erase
    auto it = std::remove_if(clientVector.begin(), clientVector.end(),
        [s](const auto& ptr) { return ptr->clientSocket == s; });
    if (it != clientVector.end()) {
        // **Manually close socket BEFORE erasing**
        closesocket(s);
        clientVector.erase(it, clientVector.end());
    }
}
