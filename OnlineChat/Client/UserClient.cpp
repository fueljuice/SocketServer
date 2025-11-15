#include "UserClient.h"


#define INTSIZE		4
#ifdef PR_DEBUG
#define DBG(X) std::cout << x << std::endl
#else
#define DBG(X)
#endif

Client::UserClient::UserClient(int domain, int service, int protocol, int port, u_long network_interface)
	:
	ClientInterface(domain, service, protocol, port, network_interface)
{
	DBG("UserClient ctor called ");
}


// determines which request user wanted to use
void Client::UserClient::sendPacket(const char* msg, u_int requestType)
{
	DBG("sending");
	
	// switching request type
	switch (requestType)
	{

	// requestType: getchat = 1, sendmessage = 2
	case 1:
	{
		getChat(requestType);
		break;
	}

	case 2:
	{
		sendMessage(strlen(msg), msg, requestType);
		break;
	}

	default:
	{
		DBG("failed to identify request");

		return;
	}





	}



}

// recving the answer from the server
std::string Client::UserClient::recievePacket()
{
	int intLength, bytesRead;
	char* endptr, * msgBuf;
	char lengthBuf[INTSIZE + 1] = { 0 };
	std::string stringMsg;

	// recving the length header
	DBG("recving packet");

	bytesRead = recv(conSocket.get()->getSock(), lengthBuf, INTSIZE, MSG_WAITALL);

	DBG("recved bytes :" << bytesRead);
	DBG("length buf :" << lengthBuf);

	intLength = strtol(lengthBuf, &endptr, 10);

	// if the length header is invalid, returns an empty string
	if (lengthBuf == endptr || bytesRead <= 0)
	{
		DBG("failed extract length from header ");

		return {};

	}
	DBG("sucsess length extraction :" << bytesRead);

	// allocates a message at the size of the length got from the header
	msgBuf = new char[intLength + 1];


	bytesRead = recv(conSocket.get()->getSock(), msgBuf, intLength, MSG_WAITALL);

	msgBuf[intLength] = '\0';

	stringMsg = msgBuf;
	delete[] msgBuf;

	if (bytesRead <= 0)
		return {};

	std::cout << "msg got:\n" << stringMsg << std::endl;
	return stringMsg;



}

// sendmessage request. sends a message to the server text file
void Client::UserClient::sendMessage(u_int msgLength, const char* msg, u_int requestType)
{
	DBG("send msg request");

	// if thers no space to pass it in 4 bytes
	if (msgLength > 9999)
	{
		std::cerr << " length too long" << std::endl;
		return;
	}

	constexpr size_t HEADER_SIZE = 2 * INTSIZE; // 8 bytes for header

	// header + message length
	size_t payloadLength = HEADER_SIZE + msgLength;

	char headerBuf[HEADER_SIZE + 1] = { 0 };
	char* payload = new char[payloadLength];


	// write header (8 bytes  for sprintf_s)
	sprintf_s(headerBuf, sizeof(headerBuf), "%0*u%0*u", INTSIZE, msgLength, INTSIZE, requestType);

	// copy header to payload
	memcpy(payload, headerBuf, HEADER_SIZE);

	// copy the message after the header
	memcpy(payload + HEADER_SIZE, msg, msgLength);


	bool isSent = sendAll(conSocket.get()->getSock(), payload, static_cast<u_int>(payloadLength));
	DBG("isSent: " << isSent);
	delete[] payload;
}

// requests the entire content for the text file from the server
void Client::UserClient::getChat(u_int requestType)
{
	DBG("get chat request");

	// header 
	constexpr size_t HEADER_SIZE = 2 * INTSIZE; // 8 bytes for header
	size_t payloadLength = HEADER_SIZE;

	char* payload = new char[payloadLength];

	// write header (8 bytes  for sprintf_s)
	char headerBuf[HEADER_SIZE + 1] = { 0 };
	sprintf_s(headerBuf, sizeof(headerBuf), "%0*u%0*u", INTSIZE, 0, INTSIZE, requestType);

	memcpy(payload, headerBuf, HEADER_SIZE);

	// sendig payload, which is 8 bytes ( size of header, 2 INTSIZE)
	sendAll(conSocket.get()->getSock(), payload, 2 * INTSIZE);
	delete[] payload;
}


// sends to the server the payload (header + body)
bool Client::UserClient::sendAll(SOCKET s, char* buf, u_int len)
{
	printf("payload: %*s", len, buf);
	int sent = 0;
	//const char* newbuf = "00000001";
	//len = 8;
	while (sent < len)
	{
		int r = send(s, buf + sent, len - sent, 0);
		if (r <= 0)
			return false;
		sent += r;
	}
}
