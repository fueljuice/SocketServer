#pragma once
#include <mutex>
#include <unordered_map>
#include <memory>
#include <WinSock2.h>
#include <string>
#include <string_view>
#include <vector>

#include "./data/clientSocketData.h"
namespace sockets::server
{


struct ISessionManager
{
	virtual ~ISessionManager() = default;

	virtual bool addClient(SOCKET socket, sockaddr clientAddr) = 0;
	virtual bool removeClient(SOCKET s) = 0;
	virtual void endSession() = 0;

	virtual std::vector<SOCKET> clientsSnapshot() const = 0;

	virtual bool setClientHeader(SOCKET s, std::string_view buf) = 0;
	virtual bool setClientData(SOCKET s, std::string_view buf) = 0;
	virtual std::string getClientHeader(SOCKET s) const = 0;
	virtual std::string getClientData(SOCKET s) const = 0;
};
class SessionManager : public ISessionManager
{
public:
	bool removeClient(SOCKET s) override;
	bool addClient(SOCKET socket, sockaddr clientAddr) override;
	void endSession() override;

	// getters
	std::string getClientHeader(SOCKET s) const override;
	std::string getClientData(SOCKET s) const override;
	std::vector<SOCKET> clientsSnapshot() const override;

	// setters
	bool setClientHeader(SOCKET s, std::string_view buf) override;
	bool setClientData(SOCKET s, std::string_view buf) override;

private:
	mutable std::mutex clientsMetaDataMutex;
	std::unordered_map<SOCKET, std::unique_ptr<data::ClientSocketData>> clientsMetaData;
};
}
