#include "TestServer.h"

void HDE::TestServer::launch()
{
    int iResult;
    std::thread worker1(handleConnection);
    std::thread worker2(responder);
    for (;;)
    {
        for (clientSocketData data : clientVector)
        {

            do {

                iResult = recv(data.clientSocket, data.buffer, sizeof(data.buffer), 0);

                if (iResult == 0)
                {
                    // remove it if connection is closed
                    printf("Connection closed\n");
                    clientVector.erase(std::remove(clientVector.begin(), clientVector.end(), data),
                        clientVector.end());
                }
                else
                    printf("recv failed: %d\n", WSAGetLastError());

            } while (iResult > 0);

            
        }

    }

}




HDE::clientSocketData HDE::TestServer::acceptConnection()
{
    sockaddr clientSock{};
    socklen_t addrLen = sizeof(clientSock);
    int newSock;
    newSock = accept(
        lstnSocket->getSock(),
        reinterpret_cast<sockaddr*>(&clientSock),
        &addrLen
    );
    HDE::clientSocketData socketdata(newSock, clientSock);
    return socketdata;

}

void HDE::TestServer::handleConnection()
{
    for (;;)
    {
        HDE::clientSocketData clientdata(acceptConnection());
        if (clientdata.clientSocket >= 0)
        {
            std::lock_guard<std::mutex> lock(m); // m.lock() but unlocks in the end of the scope
            clientVector.emplace_back(std::move(clientdata));
            

        }

    }

}

void HDE::TestServer::responder()
{
    for (;;)
    {
        for (clientSocketData data : clientVector)
        {

            do
            {
                handleClientData(data.buffer);
                
            } while (true);


        }

    }
}

