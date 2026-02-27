#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <WinSock2.h>
#include <mutex>

#include "../Server/ServerException.h"
namespace sockets::server
{
class UserRegistry
{
public:
	bool eraseClient(SOCKET sock);
	bool registerUserName(SOCKET sock, std::string_view userName);


	bool isClientExist(SOCKET sock) const;
	bool isUserNameExist(std::string_view userName) const;

	std::string getUserName(SOCKET sock)  const;
	SOCKET getSocket(std::string_view targetUserName) const;

private:
	std::unordered_map<SOCKET, std::string> clientUserNameMap;
	mutable std::mutex nameMapMutex;

	bool isClientExist_unlocked(SOCKET sock) const;
	bool isUserNameExist_unlocked(std::string_view userName) const;

};
}
