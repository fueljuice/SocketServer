#include "NetworkManager.h"
#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif 

Client::NetworkManager::NetworkManager(
    int domain,
    int service,
    int protocol,
    int port,
    u_long network_interface)
    :
    conSocket(std::make_unique<sockets::ConnectingSocket>(
        domain,
        service,
        protocol,
        port,
        network_interface))
    {}

Client::NetworkManager::~NetworkManager()
{
    conSocket->stopConnection();
}

bool Client::NetworkManager::sendAll(std::string_view payload)
{
    DBG("sending: ");
    const SOCKET sock = conSocket->getSock();
    const std::size_t len = payload.size();
    int sent = 0, r;
    DBG("sending:" << payload << "socket:" << sock);
    while (sent < len)
    {
        r = send(sock, payload.data() + sent, len - sent, 0);
        if (r <= 0)
            return false;
        sent += r;
    }
    return true;
}

std::optional<std::string> Client::NetworkManager::recvAll(size_t size)
{
    std::string data;
    data.resize(size);
    const SOCKET sock = conSocket->getSock();

    size_t lengthHeaderBytes = recv(
        sock,
        data.data(),
        static_cast<int>(size),
        MSG_WAITALL);

    // verfies everything was sent
    if (size != lengthHeaderBytes)
        return std::nullopt;
    return data;
}

SOCKET Client::NetworkManager::getSock()
{
    return conSocket->getSock();
}

void Client::NetworkManager::startNetwork()
{
    conSocket->startConnect();
}

void Client::NetworkManager::closeNetwork()
{
    conSocket->stopConnection();
}
