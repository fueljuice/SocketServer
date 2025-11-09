#pragma once

#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <string>
#include <fstream>

#include "SocketServer.h"
#include "../Parser/ParsingProtocol.h"



namespace HDE
{

	class TestServer : public SocketServer
	{

		public:
			void launch();

			void stop();

			TestServer(int domain, int service, int protocol,
				int port, u_long network_interaface, int backlog);

			~TestServer();

	private:
		std::atomic_bool running{ false };
		std::thread threadPerClient;
		std::fstream dbFile;


		std::mutex sendMutex;
		std::mutex clientVectorMutex;
		std::mutex fileMutex;


		std::vector<std::thread> clientThreads;
		std::vector<std::shared_ptr<ClientSocketData>> clientVector;


		void acceptConnection();

		void handleConnection(std::shared_ptr<ClientSocketData> client);

		void onClientAccept(std::shared_ptr<ClientSocketData> client);

		void broadcast(const char* msgBuf, int msgLen) override;

		void respondToClient(std::shared_ptr<ClientSocketData> client, messaging::ParsedRequest& pr);

		void getChat(std::shared_ptr<ClientSocketData> client, messaging::ParsedRequest& pr);

		void sendMessage(std::shared_ptr<ClientSocketData> client, messaging::ParsedRequest& pr);

		bool sendAll(SOCKET s, const char* buf, int len);

		void removeDeadClient(SOCKET s);

	
	};
}

