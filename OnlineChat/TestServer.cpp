#include "TestServer.h"
#define dbFileDir   "C:\\Users\\zohar\\Desktop\\dbFile.txt"

HDE::TestServer::TestServer(int domain, int service, int protocol,
    int port, u_long network_interaface, int backlog)
    : SocketServer(domain, service, protocol,
        port, network_interaface, backlog)
{
    std::cout << "init TestServer" << std::endl;
    dbFile.open(dbFileDir, std::ios::in | std::ios::out | std::ios::app);
}

void HDE::TestServer::launch()
{
   running.store(true);
   std::thread acceptorThread(&HDE::TestServer::acceptConnection, this);
   acceptorThread.join();
    

}




void HDE::TestServer::stop()
{
    std::cout << "stopped server" << std::endl;
    running.store(false);
    lstnSocket->stopLisetning();

}


HDE::TestServer::~TestServer()
{
    stop();
    if (dbFile.is_open())
        dbFile.close();
}



void HDE::TestServer::acceptConnection()
{

    sockaddr clientSock{};
    socklen_t addrLen;
    int newSock;
    std::cout << "accepting..." << std::endl;

    while(running.load())
    {
        sockaddr clientSock{};
        addrLen = sizeof(clientSock);
        newSock = lstnSocket->acceptCon(
            reinterpret_cast<sockaddr*>(&clientSock),
            &addrLen);
        std::cout << "done accept" << std::endl;

        if (static_cast<SOCKET>(newSock) != INVALID_SOCKET)
        {
            std::cout << "accepted valid socket" << std::endl;
            auto sharedClient = std::make_shared<ClientSocketData>((newSock, clientSock, 5000));
            {
                std::lock_guard<std::mutex> lk(clientVectorMutex);  
                clientVector.push_back(sharedClient);
            }
            onClientAccept(*sharedClient);
            
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

    // getting the length of the request before iterating
    iResult = recv(
        client.clientSocket,
        client.dataBuf.get(),
        4,
        MSG_WAITALL
    );

    if (iResult != 4)
    {
        std::cout << "couldnt read from client" << std::endl;
        return;
    }

    messaging::ParsingProtocol pq(client.dataBuf.get());
    reqLength = pq.getRequestLength(); // the length of the request


    // waiting until the entire meessage arrives using the length 
    // we got and the MSG_WAITALL flag
        iResult = recv(
        client.clientSocket,
        client.dataBuf.get(),
        reqLength,
        MSG_WAITALL
    );

    if (iResult > 0)
    {
        handleClientData(client, reqLength);
        printf("Bytes received: %d\n", iResult);
    }

    else if (iResult == 0)
    {
        printf("Connection closed\n");
    }
    else
        printf("recv failed: %d\n", WSAGetLastError());

}

void HDE::TestServer::handleClientData(ClientSocketData& client, int readLength)
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
        {
            
        std::lock_guard<std::mutex> lk(clientVectorMutex);
        clientVector.erase(
            std::remove_if(
                clientVector.begin(),
                clientVector.end(),
                [&](const std::shared_ptr<ClientSocketData>& p) {
                    return p && p->clientSocket == client.clientSocket; // compare sockets
                }),
            clientVector.end());
            
        }
    }

    
}

void HDE::TestServer::sendMessage(ClientSocketData& client, messaging::ParsedRequest pr)
{
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
    std::vector<std::shared_ptr<ClientSocketData>> snapshot;
    {
        std::lock_guard<std::mutex> lk(clientVectorMutex);
        snapshot = clientVector;
    }

    for (const auto& client : snapshot)
    {

        if (!running.load())
            break;
        {
            std::lock_guard<std::mutex> lk(sendMutex);
            isSent = sendAll(
                client->clientSocket,
                msgBuf,
                msgLen
            );
        }

        if (!isSent)
        {
            deadClients.push_back(client->clientSocket);
        }
    }

    if (!deadClients.empty()) 
    {
        std::lock_guard<std::mutex> lk(clientVectorMutex);
        // removing by socket all the dead clients ( with unavaiable sockets)
        clientVector.erase(
            std::remove_if(
                clientVector.begin(),
                clientVector.end(),
                [&](const std::shared_ptr<ClientSocketData>& x)
                {
                    return std::find(deadClients.begin(), deadClients.end(), x->clientSocket) != deadClients.end();
                }),
            clientVector.end()
        );
    }
}

bool HDE::TestServer::sendAll(int s, const char* buf, int len)
{
    int sent = 0;
    while (sent < len) {
        int r = send(s, buf + sent, len - sent, 0);
        if (r == SOCKET_ERROR) return false;
        sent += r;
    }
    return true;
}

