#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <WinSock2.h>
#include <mutex>

#include "../Server/ServerException.h"
namespace sockets::server
{

struct IUserRegistry
{
	virtual ~IUserRegistry() = default;
	virtual bool eraseClient(SOCKET sock) = 0;
	virtual bool registerUserName(SOCKET sock, std::string_view userName) = 0;

	virtual bool isClientExist(SOCKET sock) const = 0;
	virtual bool isUserNameExist(std::string_view userName) const = 0;

	virtual std::string getUserName(SOCKET sock)  const = 0;
	virtual SOCKET getSocket(std::string_view targetUserName) const = 0;
};
class UserRegistry : public IUserRegistry
{
public:
	bool eraseClient(SOCKET sock) override;
	bool registerUserName(SOCKET sock, std::string_view userName) override;

	bool isClientExist(SOCKET sock) const override;
	bool isUserNameExist(std::string_view userName) const override;

	std::string getUserName(SOCKET sock)  const override;
	SOCKET getSocket(std::string_view targetUserName) const override;

private:
	std::unordered_map<SOCKET, std::string> clientUserNameMap;
	mutable std::mutex nameMapMutex;

	bool isClientExist_unlocked(SOCKET sock) const;
	bool isUserNameExist_unlocked(std::string_view userName) const;

};
}
