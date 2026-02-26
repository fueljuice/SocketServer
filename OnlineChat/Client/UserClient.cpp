#include "UserClient.h"
#include "../Protocol/ProtocolConstants.h"
#include "ClientExceptions.h"
#include <ctime>

constexpr const char* NO_RESPONSE = "NO_RESPONSE";
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

Client::UserClient::~UserClient()
{
	stopPassiveListener();
}


// determines which request user wanted to use
void Client::UserClient::sendRequest(const char* msg, u_int requestType)
{
	DBG("sending");
	sendRequestInternal(static_cast<u_int>(strlen(msg)), msg, requestType);
}

void Client::UserClient::sendRequestInternal(u_int msgLength, const char* msg, u_int requestType)
{
	// construct request
	std::string payload = messaging::ClientProtocol::constructRequest(msgLength, msg, requestType);
	// send it
	bool isSent = sendAll(conSocket.get()->getSock(), payload.c_str(), payload.size());
	DBG("is sent?: " << isSent);
}

// recving the answer from the server
std::string Client::UserClient::recieveResponse()
{
	try
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

		// check for network errors
		if (bytesRead == SOCKET_ERROR)
			throw Client::ConnectionException("Socket error while receiving header");
		if (bytesRead == 0)
			throw Client::ConnectionException("Server closed connection");

		// extracting header
		messaging::ParsedResponse pr =
			messaging::ClientProtocol::parseHeader(
				reinterpret_cast<const char*>(header.data()),
				bytesRead
			);

		DBG("data size from header: " << pr.dataSize);

		// recving data if there it exists.
		if (pr.dataSize == 0)
			return NO_RESPONSE;

		// recving data
		DBG("recving data from server...");
		msgBuf = new char[pr.dataSize + 1];
		bytesRead = recv(conSocket.get()->getSock(), msgBuf, pr.dataSize, MSG_WAITALL);

		// check for network errors
		if (bytesRead == SOCKET_ERROR)
		{
			delete[] msgBuf;
			throw Client::ConnectionException("Socket error while receiving data");
		}
		if (bytesRead != pr.dataSize)
		{
			delete[] msgBuf;
			throw Client::DataCorruptionException("expected " + std::to_string(pr.dataSize) +
				" bytes, received: " + std::to_string(bytesRead));
		}

		messaging::ParsedResponse refindPR = messaging::ClientProtocol::parseData(
			std::move(pr),
			reinterpret_cast<const char*>(msgBuf)
		);
		delete[] msgBuf;
		return refindPR.dataBuffer;


	}
	catch (const Client::ProtocolException& e)
	{
		// rethrow protocol exceptions
		throw;
	}
	catch (const std::exception& e)
	{
		throw Client::ClientException("Error in receiveResponse: " + std::string(e.what()));
	}
}


// sends to the server the payload (header + body)
bool Client::UserClient::sendAll(SOCKET s, const char* buf, u_int len)
{
	int sent = 0;
	while (sent < len)
	{
		int r = send(s, buf + sent, len - sent, 0);
		if (r <= 0) {
			throw Client::ConnectionException("Failed to send data to server");
		}
		sent += r;
	}
	return true;
}

// passive listening implementation
void Client::UserClient::startPassiveListener()
{
	if (shouldListen.load())
		return; // already listening

	shouldListen.store(true);
	listenerThread = std::thread(&UserClient::passiveListenLoop, this);
}

void Client::UserClient::stopPassiveListener()
{
	shouldListen.store(false);
	// stops thread
	if (listenerThread.joinable())
		listenerThread.join();
}


void Client::UserClient::passiveListenLoop()
{
	while (shouldListen.load())
	{
		try
		{
			if (checkForMessages())
			{
				// if we found messages, sleep a bit to allow processing
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			else
			{
				// no messages, sleep longer to reduce cpu usage
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		catch (const std::exception& e)
		{
			// log error but continue listening
			DBG("passive listener error: " << e.what());
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
}

bool Client::UserClient::checkForMessages()
{
	// sets a a 
	fd_set readSet;
	FD_ZERO(&readSet);
	FD_SET(conSocket->getSock(), &readSet);

	// set timeout to 0 
	timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 1;

	// check for readable sockets
	int result = select(0, &readSet, nullptr, nullptr, &timeout);
	if (result == SOCKET_ERROR)
		return false; // socket error
	// if 
	if (result > 0 && FD_ISSET(conSocket->getSock(), &readSet))
	{
		// socket has data, try to receive it
		try
		{
			// recvs response from server
			std::string response = recieveResponse();
			if (!response.empty() && response != NO_RESPONSE)
			{
				std::cout << "\n[NEW MESSAGE] " << response << std::endl;
				return true;
			}
		}
		catch (const Client::ConnectionException& e)
		{
			// connection error, stop listening
			DBG("connection error in passive listener: " << e.what());
			shouldListen.store(false);
			return false;
		}
		catch (const std::exception& e)
		{
			// other error, log but continue
			DBG("error receiving message: " << e.what());
			return false;
		}
	}

	return false;
}
