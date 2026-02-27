#pragma once
#include <mutex>
#include <string>
#include <string_view>
#include <WinSock2.h>

#include "../Protocol/ServerProtocol.h"
#include "../Protocol/ProtocolConstants.h"
namespace sockets::server
{
	class NetworkIO final
{
public:
	// TODO: ADD STATUS CODE IN THE RESPONSE
	bool sendAll(SOCKET sock, std::string_view payload);
	std::string recvAll(SOCKET sock, size_t size);
	bool sendError(SOCKET sock, int statusCode);

private:
	std::mutex sendMutex;
};
}

