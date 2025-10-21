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
	std::mutex m;
	std::vector<clientSocketData> clientVector;
	clientSocketData acceptConnection() override;

	void handleConnection();

	void responder();

public:
	void launch();

	TestServer();
};
}

