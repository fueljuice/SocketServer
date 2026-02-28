#include "SessionManager.h"


bool sockets::server::SessionManager::removeClient(SOCKET s)
{
    std::lock_guard<std::mutex> lk(clientsMetaDataMutex);
	return static_cast<bool>(clientsMetaData.erase(s));
    
}

bool sockets::server::SessionManager::addClient(SOCKET socket, sockaddr clientAddr)
{
    std::lock_guard<std::mutex> lk(clientsMetaDataMutex);
    if (socket == INVALID_SOCKET)
        return false;
        
    clientsMetaData[socket] = std::make_unique<data::ClientSocketData>(socket, clientAddr);
    return true;
}

void sockets::server::SessionManager::endSession()
{
    // clearing each socket cached
    std::lock_guard<std::mutex> lk(clientsMetaDataMutex);
    for (auto& [s, client]:clientsMetaData)
        client.reset();
	clientsMetaData.clear();
}

std::string sockets::server::SessionManager::getClientData(SOCKET s) const
{
	std::lock_guard<std::mutex> lk(clientsMetaDataMutex);
    auto it = clientsMetaData.find(s);
	if (it != clientsMetaData.end())
		return it->second->dataBuf;
    return "";
}

std::vector<SOCKET> sockets::server::SessionManager::clientsSnapshot() const
{
    std::lock_guard<std::mutex> lk(clientsMetaDataMutex);
    std::vector<SOCKET> snapshot;
    snapshot.reserve(clientsMetaData.size());
    for (const auto& [sock, metadata] : clientsMetaData)
        snapshot.push_back(sock);
    return snapshot;
}


bool sockets::server::SessionManager::setClientData(SOCKET s, std::string_view buf)
{
    std::lock_guard<std::mutex> lk(clientsMetaDataMutex);
    auto it = clientsMetaData.find(s);
    if (it == clientsMetaData.end())
        return false;

    it->second->dataBuf = buf;
    return true;
}

std::string sockets::server::SessionManager::getClientHeader(SOCKET s) const
{
    std::lock_guard<std::mutex> lk(clientsMetaDataMutex);
    auto it = clientsMetaData.find(s);
    if (it != clientsMetaData.end())
        return it->second->headerBuf;
    return "";
}

bool sockets::server::SessionManager::setClientHeader(SOCKET s, std::string_view buf)
{
    std::lock_guard<std::mutex> lk(clientsMetaDataMutex);
    auto it = clientsMetaData.find(s);
    if (it == clientsMetaData.end())
        return false;
    it->second->headerBuf = buf;
    return true;
}
