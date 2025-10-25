#pragma once
#include "ConnectingSocket.h"

class ClientInterface : public HDE::ConnectingSocket
{

private:

protected:
	char* m_data;
	int serverSock;

public:
	virtual ~ClientInterface() = 0;

	virtual void sendPacket() = 0;

	virtual void recievePacket() = 0;

};
