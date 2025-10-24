#pragma once
#include "SocketServer.h"
#include "FJProtocol.h"
#include <vector>
#include <mutex>
#include <thread>
#include <stdlib.h>
#include <cstring>
#include <string>

//TODO: need to contsntily check if new messages arrived.
// if it does send them to all current connections



namespace HDE
{



class TestServer : public SocketServer
{
private:
	enum action
	{
		GETCHAT = 1,
		SENDMESSAGE = 2,
		SETNAME = 3

	};

	std::mutex m;
	std::vector<clientSocketData> clientVector;


	clientSocketData acceptConnection() override;

	void handleConnection();

	void responder();

	action handleClientData(const char* buffer, int dataLength);

public:
	void launch();



	TestServer(int domain, int service, int protocol,
		int port, u_long network_interaface, int backlog) :

		SocketServer(domain, service, protocol,
		 port,  network_interaface, backlog) 
	{
		std::cout << "init TestServer" << std::endl;
		launch();
	}

	
};
}

