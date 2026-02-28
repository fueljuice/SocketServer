#pragma once
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>
#include <WinSock2.h>
#include <optional>

namespace sockets::server
{

struct INetworkIO
{
	virtual ~INetworkIO() = default;
	virtual bool sendAll(SOCKET sock, std::string_view payload) = 0;
	virtual std::optional<std::string> recvAll(SOCKET sock, size_t size) = 0;
};
class NetworkIO : public INetworkIO
{
public:
	// TODO: ADD STATUS CODE IN THE RESPONSE
	bool sendAll(SOCKET sock, std::string_view payload);
	std::optional<std::string> recvAll(SOCKET sock, size_t size);
	bool sendError(SOCKET sock, int statusCode);

private:
	std::mutex sendMutex;
};
}

