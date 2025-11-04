#pragma once

#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <string>
#include <fstream>

#include "SocketServer.h"
#include "ParsingProtocol.h"




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
		std::vector<SOCKET> clientVector;


		void acceptConnection() override;

		void handleConnection(ClientSocketData client) override;

		void onClientAccept(ClientSocketData& client) override;

		void broadcast(const char* msgBuf, int msgLen) override;

		void respondToClient(ClientSocketData& client, int readLength) override;

		void getChat(ClientSocketData& client, messaging::ParsedRequest pr);

		void sendMessage(ClientSocketData& client, messaging::ParsedRequest pr);

		bool sendAll(SOCKET s, const char* buf, int len);

	
	};
}

