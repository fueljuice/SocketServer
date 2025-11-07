#pragma once

#include "SimpleSocket.h"

namespace HDE
{

	class ListeningSocket : public SimpleSocket
	{
		public:
			ListeningSocket(int domain, int service, int protocol, int port, u_long network_interaface, int backlog);

			void connectToNetwork(SOCKET sock, struct sockaddr_in address) override;

			SOCKET bindSocket(SOCKET sock, struct sockaddr_in address);

			void startLisetning();

			void stopLisetning();

			SOCKET acceptCon(sockaddr* address, int* addlen);



		private:
			int backlog;
			int listening;
	};
		


}