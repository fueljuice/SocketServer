#pragma once
#include "../Sockets/ListeningSocket.h"
#include "../clientSocketData.h"

namespace HDE
{


	class SocketServer
	{

		public:
			SocketServer(int domain, int service, int protocol,
				int port, u_long network_interaface, int backlog);


			//virtual void launch();

			//virtual void stop();


			// rule of three
			virtual ~SocketServer() = default;
			SocketServer(const SocketServer&) = delete;  // unique_ptr  noncopyable
			SocketServer& operator=(const SocketServer&) = delete;


		protected:
			std::unique_ptr<ListeningSocket> lstnSocket;


		private:
			//virtual void acceptConnection() = 0;

			//virtual void handleConnection(ClientSocketData client) = 0;

			//virtual void respondToClient(ClientSocketData& client, int readLength) = 0;

			virtual void broadcast(const char* msgBuf, int msgLen) = 0;



	};
}