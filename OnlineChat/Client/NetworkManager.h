#pragma once
#include <string>
#include <string_view>
#include <WinSock2.h>
#include <optional>
#include <memory>
#include <mutex>

#include "../Sockets/ConnectingSocket.h"

namespace Client
{

struct INetworkManager
{
	virtual ~INetworkManager() = default;
	virtual void startNetwork() = 0;
	virtual void closeNetwork() = 0;

	// network IO functions
	virtual bool sendAll(std::string_view payload) = 0;
	virtual std::optional<std::string> recvAll(size_t size) = 0;

	// getter
	virtual SOCKET getSock() = 0;

};
class NetworkManager : public INetworkManager
{
public:
	NetworkManager(int domain, int service, int protocol, int port, u_long network_interface);
	~NetworkManager();

	// open / close connection
	void startNetwork();
	void closeNetwork();

	// network IO functions
	bool sendAll(std::string_view payload);
	std::optional<std::string> recvAll(size_t size);
	
	// getter
	SOCKET getSock();
private:
	std::unique_ptr<sockets::ConnectingSocket> conSocket;
	

};
}

