#include "ClientProtocol.h"
#include "ProtocolConstants.h"
#include "../Client/ClientExceptions.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

messaging::ParsedResponse messaging::ClientProtocol::parseHeader(std::string rawHeader, unsigned int rawLength)
{
	ParsedResponse pr;
	// header must be the correct length
	if (rawLength != RESPONSE_HEADER_SIZE)
	{
		DBG("obsufcated response header");
		throw Client::InvalidHeaderException("header size mismatch: expected " + std::to_string(RESPONSE_HEADER_SIZE)
			+ "bytes and only got: " + std::to_string(rawLength));
	}
	extractLength(pr, rawHeader, rawLength);
	return pr;
}

messaging::ParsedResponse messaging::ClientProtocol::parseData(ParsedResponse&& pr, std::string rawData)
{
	DBG("parsing data");
	if (pr.dataSize == -1)
		return pr;

	pr.dataBuffer.reserve(pr.dataSize);
	pr.dataBuffer.assign(rawData.begin(), rawData.begin() + pr.dataSize);
	return pr;
}

std::string messaging::ClientProtocol::constructRequest(unsigned int msgLength, std::string msg, std::string recver, ActionType requestType)
{

	// calculate total payload size
	const int payloadLength = REQUEST_HEADER_SIZE + msgLength;
	// create payload
	std::string payload;
	payload.reserve(payloadLength);

	std::string header = constructHeader(msgLength, requestType);
	std::string data = constructData(msgLength, msg, recver, requestType);

	return header + data;
}

std::string messaging::ClientProtocol::constructHeader(unsigned int msgLength, ActionType requestType)
{
	// check for msg length
	if (msgLength > MAX_MESSAGE_LENGTH)
	{
		std::cerr << "Message length too long (max: " << MAX_MESSAGE_LENGTH << ")" << std::endl;
		return {};
	}

	// construct header
	char headerBuf[REQUEST_HEADER_SIZE + 1] = { 0 };
	sprintf_s(headerBuf, sizeof(headerBuf), "%0*u%0*u%u", REQUEST_DATA_LENGTH_SIZE, msgLength, REQUEST_TYPE_SIZE, requestType, PROTOCOL_VERSION);
	std::cout << "constructed header: " << headerBuf << std::endl;
	return std::string(headerBuf, REQUEST_HEADER_SIZE);
}

std::string messaging::ClientProtocol::constructData(unsigned int msgLength, std::string msg, std::string recver, ActionType requestType)
{
	std::string data;
	// sepreate data and username
	if (requestType == messaging::ActionType::DIRECT_MESSAGE)
	{
		// must provide a recver for direct messages
		if (recver.empty() || recver.size() == 0)
			throw Client::DataCorruptionException("Receiver username cannot be empty for direct messages");
		return std::string(recver) + REQUEST_DATA_SEPERATOR + std::string(msg, msgLength);
	}
	return std::string(msg, msgLength);
}

void messaging::ClientProtocol::extractLength(ParsedResponse& pr, std::string rawHeader, unsigned int rawLength)
{
	int intLength;
	char* endptr;
	const char* cRawHeader = rawHeader.c_str();
	intLength = strtol(cRawHeader, &endptr, 10);

	// if the length header is invalid
	if (cRawHeader == endptr)
	{
		DBG("failed extract length from header ");
		throw Client::InvalidHeaderException("Failed to extract length from header");
	}

	// check if length is ok
	if (intLength < 0 || intLength > MAX_MESSAGE_LENGTH)
	{
		DBG("invalid length value: " << intLength);
		throw Client::InvalidHeaderException("Invalid length value: " + std::to_string(intLength));
	}
	// sucsessful extraction
	pr.dataSize = intLength;
	DBG("sucsess length extraction :" << intLength);
}


