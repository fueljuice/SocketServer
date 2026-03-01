#pragma once

#include <WinSock2.h>
#include <memory>

namespace sockets::server
{
class Server;

class ServerBuilder
{
public:
	ServerBuilder(int domain, int service, int protocol,
		int port, u_long network_interaface, int backlog);

	std::unique_ptr<Server> build();

private:
	int domain;
	int service;
	int protocol;
	int port;
	u_long network_interaface;
	int backlog;
};
}
