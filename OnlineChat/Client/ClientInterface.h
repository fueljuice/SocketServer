#pragma once
#include "/ConnectingSocket.h"

class ClientInterface : public HDE::ConnectingSocket
{
public:
	virtual ~ClientInterface() = 0;

	virtual void sendPacket() = 0;

	virtual void recievePacket() = 0;

protected:
	char* m_data;
	SOCKET serverSock;

private:


};
