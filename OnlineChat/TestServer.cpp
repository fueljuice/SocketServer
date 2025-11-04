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

    sockaddr clientSock{};
    socklen_t addrLen;
    SOCKET newSock;
    std::cout << "accepting..." << std::endl;

    // running while the atomic member is true (on).
    while(running.load())
    {
        sockaddr clientSock{};
        addrLen = sizeof(clientSock);

        // async function that waits for a client
        newSock = lstnSocket->acceptCon(
            reinterpret_cast<sockaddr*>(&clientSock),
            &addrLen);

        std::cout << "done accept..." << std::endl;

        // in the case of a legit socket
        if (newSock != INVALID_SOCKET)
        {
            std::cout << "accepted valid socket" << std::endl;
            ClientSocketData client(newSock, clientSock, 5000);
            {
                // locking before pushing the socket of the client into the list of clients.
                // the list is later used to brod
                std::lock_guard<std::mutex> lk(clientVectorMutex);  
                clientVector.push_back(client.h->socketHandle);
            }
            onClientAccept(client);
        
        }
        else
        {
            std::cout << "accpeted invalid socket" << std::endl;
        }
    }

}

void HDE::TestServer::onClientAccept(ClientSocketData& client)
{
    std::thread threadPerClient(
        &HDE::TestServer::handleConnection,
        this,
        std::move(client));
        threadPerClient.detach();

}



void HDE::TestServer::handleConnection(ClientSocketData client)
{
    int iResult, totalrecv = 0, reqLength;
    std::cout << "is client up?: " << client.clientSocket << std::endl;
    bool isup = client.clientSocket == INVALID_SOCKET;
    std::cout << "is client socket invalid? : " << isup << std::endl;

    // getting the length of the request before iterating
    iResult = recv(
        client.clientSocket,
        client.dataBuf.get(),
        4,
        MSG_WAITALL
    );
    printf("buf: %s\n", client.dataBuf.get());

    if (iResult != 4)
    {
        std::cout << "couldnt read length from client. bytes read: " << iResult << std::endl;
        std::cout << "last problem: " << WSAGetLastError() << std::endl;
        return;
    }

    messaging::ParsingProtocol pq(client.dataBuf.get());
    reqLength = pq.getRequestLength(); // the length of the request
    std::cout << "length: : " << reqLength << std::endl;

    // waiting until the entire meessage arrives using the length 
    // we got and the MSG_WAITALL flag
    std::cout << "recving from client" << std::endl;
        iResult = recv(
        client.clientSocket,
        client.dataBuf.get(),
        reqLength,
        MSG_WAITALL
    );
        std::cout << "done rcev" << std::endl;
        
    if (iResult > 0)
    {
        respondToClient(client, reqLength);
        printf("Bytes received: %d\n", iResult);
    }

    else if (iResult == 0)
    {
        printf("Connection closed\n");
    }
    else
        printf("recv failed: %d\n", WSAGetLastError());

}

void HDE::TestServer::respondToClient(ClientSocketData& client, int readLength)
{

    const char* buffer = client.dataBuf.get();
    int bufLength = readLength;
    messaging::ParsingProtocol pq(buffer, bufLength);
    messaging::ParsedRequest pr = pq.enforceProtocol();

    if (pr.statusCode == 404)
    {
        std::cout << "STATUS CODE BAD 404" << std::endl;
    }
    else
    {
        std::cout << "STATUS CODE OK 200" << std::endl;
        switch (pr.requestType)
        {

            case messaging::SENDMESSAGE:
            {
                sendMessage(client, pr);
                break;
            }

            case messaging::GETCHAT:
            {
                getChat(client, pr);
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

void HDE::TestServer::getChat(ClientSocketData& client, messaging::ParsedRequest pr)
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
            client.clientSocket,
            allText.c_str(),
            static_cast<int>(allText.size())
        );
    }

    if (!isSent)
    {
        std::cout << "client socket unavaiable. cant send client" << std::endl;
        // Move the lock_guard outside the erase call to ensure the lock is held during erase
        {
            auto rm = std::remove(clientVector.begin(), clientVector.end(), client.clientSocket);
            std::lock_guard<std::mutex> lk(clientVectorMutex);
            clientVector.erase(rm, clientVector.end());
        }
    }

}

void HDE::TestServer::sendMessage(ClientSocketData& client, messaging::ParsedRequest pr)
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
    std::vector<SOCKET> snapShot;
    {
        std::lock_guard<std::mutex> lk(clientVectorMutex);
        snapShot = clientVector;
    }

    for (const auto& clientSocket : snapShot)
    {

        if (!running.load())
            break;
        {
            std::lock_guard<std::mutex> lk(sendMutex);
            isSent = sendAll(
                clientSocket,
                msgBuf,
                msgLen
            );
        }

        if (!isSent)
        {
            deadClients.push_back(clientSocket);
        }
    }

    if (!deadClients.empty()) 
    {
        // removing by socket all the dead clients ( with unavaiable sockets)
        for (SOCKET deadClientSocket : deadClients)
        {
            auto rm = std::remove(clientVector.begin(), clientVector.end(), deadClientSocket);
            std::lock_guard<std::mutex> lk(clientVectorMutex);
            clientVector.erase(rm, clientVector.end());
        }
    }
}

bool HDE::TestServer::sendAll(SOCKET s, const char* buf, int len)
{
    int sent = 0;
    while (sent < len) {
        int r = send(s, buf + sent, len - sent, 0);
        if (r == SOCKET_ERROR) return false;
        sent += r;
    }
    return true;
}

