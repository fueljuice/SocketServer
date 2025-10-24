#pragma once

#include "ConnectingSocket.h"
class ClientInterface : public HDE::ConnectingSocket
{

private:

protected: 
	char* m_data;

public:
	virtual void sendPacket() = 0;

	virtual void recievePacket() = 0;

};