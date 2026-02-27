#include "UserRegistry.h"

bool sockets::server::UserRegistry::isClientExist(SOCKET sock) const
{
	std::lock_guard<std::mutex> lk(nameMapMutex);
    return clientUserNameMap.find(sock) != clientUserNameMap.end();
}

bool sockets::server::UserRegistry::isUserNameExist(std::string_view userName) const
{
    std::lock_guard<std::mutex> lk(nameMapMutex);
    for (const auto& [client, curUserName] : clientUserNameMap)
    {
        if (curUserName == std::string(userName))
            return true;
    }
    return false;
}

bool sockets::server::UserRegistry::registerUserName(SOCKET sock, std::string_view userName)
{
    std::lock_guard<std::mutex> lk(nameMapMutex);
    // must be unregistered and name must be clear
    if(isClientExist_unlocked(sock) || isUserNameExist_unlocked(userName) || userName.empty())
        return false;

    clientUserNameMap[sock] = userName;
    return true;

}

bool sockets::server::UserRegistry::eraseClient(SOCKET sock)
{
    std::lock_guard<std::mutex> lk(nameMapMutex);
    // client must exis
    if(!isClientExist_unlocked(sock))
        return false;

    clientUserNameMap.erase(sock);
    return true;
}

std::string sockets::server::UserRegistry::getUserName(SOCKET sock) const
{
    std::lock_guard<std::mutex> lk(nameMapMutex);
    if(clientUserNameMap.find(sock) == clientUserNameMap.end())
		return "";
    return clientUserNameMap.at(sock);
}

SOCKET sockets::server::UserRegistry::getSocket(std::string_view targetUserName) const
{
    std::lock_guard<std::mutex> lk(nameMapMutex);
    for (const auto& [curSocket, curUserName] : clientUserNameMap)
    {
        if (std::string_view(curUserName) == targetUserName)
        {
            return curSocket;
        }
    }
    return INVALID_SOCKET;
}

bool sockets::server::UserRegistry::isClientExist_unlocked(SOCKET sock) const
{
    return clientUserNameMap.find(sock) != clientUserNameMap.end();
}

bool sockets::server::UserRegistry::isUserNameExist_unlocked(std::string_view userName) const
{
    for (const auto& [client, curUserName] : clientUserNameMap)
    {
        if (curUserName == std::string(userName))
            return true;
    }
    return false;
}
