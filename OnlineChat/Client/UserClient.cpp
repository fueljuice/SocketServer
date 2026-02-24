#include "UserClient.h"
#include "../Protocol/ProtocolConstants.h"
#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif

Client::UserClient::UserClient(int domain, int service, int protocol, int port, u_long network_interface)
	:
	AbstractClient(domain, service, protocol, port, network_interface)
{
	DBG("UserClient ctor called. ");
}


// determines which request user wanted to use
void Client::UserClient::sendRequest(const char* msg, u_int requestType, const char* userName)
{
	DBG("sending");
	sendRequestInternal(static_cast<u_int>(strlen(msg)), msg, requestType, userName);
}

void Client::UserClient::sendRequestInternal(u_int msgLength, const char* msg, u_int requestType, const char* userName)
{
	// construct request
	std::string payload = messaging::ClientProtocol::constructRequest(msgLength, msg, requestType, userName);
	// send it
	bool isSent = sendAll(conSocket.get()->getSock(), payload.c_str(), payload.size());
	DBG("is sent?: " << isSent);
}

// recving the answer from the server
std::string Client::UserClient::recieveResponse()
{
	int bytesRead;
	char* endptr, * msgBuf = { 0 };
	std::array<std::byte, messaging::RESPONSE_HEADER_SIZE> header{};
	std::string stringMsg;

	DBG("recving packet");

	// recving from server the header for the length
	bytesRead = recv(
		conSocket.get()->getSock(),
		reinterpret_cast<char*>(header.data()),
		messaging::REQUEST_DATA_LENGTH_SIZE,
		MSG_WAITALL);

	// extracting header
	messaging::ParsedResponse pr =
		messaging::ClientProtocol::parseHeader(
			reinterpret_cast<const char*>(header.data()),
			bytesRead
		);


	DBG("data size from header: " << pr.dataSize);


	// if header is invalid
	if (pr.dataSize == -1)
	{
		DBG("invalid header");
		return "server returned invalid header";
	}

	// recving data if there it exists.
	if (pr.dataSize == 0)
		return "server responded only header";


	DBG("recving data from server...");
	msgBuf = new char[pr.dataSize + 1];
	bytesRead = recv(conSocket.get()->getSock(), msgBuf, pr.dataSize, MSG_WAITALL);
	messaging::ParsedResponse refindPR = messaging::ClientProtocol::parseData(
		std::move(pr),
		reinterpret_cast<const char*>(msgBuf)
	);
	delete[] msgBuf;
	return refindPR.dataBuffer;


}


// sends to the server the payload (header + body)
bool Client::UserClient::sendAll(SOCKET s, const char* buf, u_int len)
{
	int sent = 0;
	while (sent < len)
	{
		int r = send(s, buf + sent, len - sent, 0);
		if (r <= 0)
			return false;
		sent += r;
	}
	return true;
}
