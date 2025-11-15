#pragma once


#include <memory>

#include "../Sockets/ConnectingSocket.h"
namespace Client
{
	class ClientInterface
	{
	public:

		ClientInterface(int domain, int service, int protocol, int port, u_long network_interface);


		// rule of three
		virtual ~ClientInterface();
		// delete copy constructors and asignment operators because of the a unique ptr
		ClientInterface(const ClientInterface&) = delete;
		ClientInterface& operator=(const ClientInterface&) = delete;

		virtual void sendPacket(const char* msg, u_int requestType) = 0;

		virtual std::string recievePacket() = 0;

	protected:
		std::unique_ptr<sockets::ConnectingSocket> conSocket;

	};
}
