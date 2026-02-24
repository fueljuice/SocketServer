#include "ClientProtocol.h"
#include "ProtocolConstants.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

messaging::ParsedResponse messaging::ClientProtocol::parseHeader(const char* rawHeader, unsigned int rawLength)
{
	ParsedResponse pr;
	if (rawLength != RESPONSE_HEADER_SIZE)
	{
		DBG("obsufcated response header");
		return pr;
	}
	extractLength(pr, rawHeader, rawLength);
	return pr;
}

messaging::ParsedResponse messaging::ClientProtocol::parseData(ParsedResponse&& pr, const char* rawData)
{
	DBG("parsing data");
	if (pr.dataSize == -1)
	{
		return pr;
	}
	pr.dataBuffer.reserve(pr.dataSize);
	pr.dataBuffer.assign(rawData, rawData + pr.dataSize);
	return pr;
}

std::string messaging::ClientProtocol::constructRequest(unsigned int msgLength, const char* msg, unsigned int requestType, const char* name)
{
	
	// check for msg length
	if (msgLength > MAX_MESSAGE_LENGTH)
	{
		std::cerr << "Message length too long (max: " << MAX_MESSAGE_LENGTH << ")" << std::endl;
		return {};
	}
	
	// check for username length
	if (!name || strlen(name) > USERNAME_SIZE)
	{
		std::cerr << "Invalid username" << std::endl;
		return {};
	}

	// calculate total payload size
	const int payloadLength = REQUEST_HEADER_SIZE + msgLength;
	
	// create payload
	std::string payload;
	payload.reserve(payloadLength);
	
	// construct header
	char headerBuf[REQUEST_HEADER_SIZE + 1] = {0};
	sprintf_s(headerBuf, sizeof(headerBuf), "%0*u%0*u", REQUEST_DATA_LENGTH_SIZE, msgLength, REQUEST_TYPE_SIZE, requestType);
	
	// copy username to header (with proper bounds checking)
	memcpy(headerBuf + USERNAME_OFFSET, name, std::min(strlen(name), static_cast<size_t>(USERNAME_SIZE)));
	
	// Build complete payload
	payload.append(headerBuf, REQUEST_HEADER_SIZE);
	if (msg && msgLength > 0)
	{
		payload.append(msg, msgLength);
	}
	std::cout << payload << std::endl;
	return payload;
}

void messaging::ClientProtocol::extractLength(ParsedResponse& pr, const char* rawHeader, unsigned int rawLength)
{
	int intLength;
	char* endptr;
	intLength = strtol(rawHeader, &endptr, 10);

	// if the length header is invalid, returns an empty string
	if (rawHeader == endptr || rawLength <= 0)
	{
		DBG("failed extract length from header ");
		return;
	}
	pr.dataSize = intLength;
	DBG("sucsess length extraction :" << intLength);

}


