#pragma once
#include <mutex>
#include <unordered_map>
#include <memory>
#include <WinSock2.h>
#include <string>
#include <string_view>

#include "./data/clientSocketData.h"
namespace sockets::server
{
class SessionManager
{
public:
	bool removeClient(SOCKET s);
	bool addClient(SOCKET socket, sockaddr clientAddr);
	void endSession();

	// getters
	std::string getClientHeader(SOCKET s) const;
	std::string getClientData(SOCKET s) const;
	SOCKET getClientSocketByIndex(size_t idx) const;

	// setters
	bool setClientHeader(SOCKET s, std::string_view buf);
	bool setClientData(SOCKET s, std::string_view buf);

private:
	mutable std::mutex clientsMetaDataMutex;
	std::unordered_map<SOCKET, std::unique_ptr<data::ClientSocketData>> clientsMetaData;
};
}
