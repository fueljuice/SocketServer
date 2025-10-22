#pragma once
#include "SocketServer.h"
#include <vector>
#include <mutex>
#include <thread>


	

namespace HDE
{

class TestServer : public SocketServer
{
private:
	enum action
	{
		GETCHAT,
		SENDMESSAGE

	};

	std::mutex m;
	std::vector<clientSocketData> clientVector;
	clientSocketData acceptConnection() override;

	void handleConnection();

	void responder();

	action handleClientData(char* buffer);

public:
	void launch();

	TestServer();
};
}

