#include "ClientProtocol.h"
#include "ProtocolConstants.h"
#include "../Client/ClientExceptions.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

std::optional<messaging::ParsedResponse> messaging::ClientProtocol::parseHeader(std::string_view rawHeader, size_t rawLength)
{
	ParsedResponse pr;
	// header must be the correct length
	if (rawLength != RESPONSE_HEADER_SIZE)
		return std::nullopt;

	extractLength(pr, rawHeader);
	return pr;
}

std::optional <messaging::ParsedResponse> messaging::ClientProtocol::parseData(ParsedResponse&& pr, std::string rawData)
{
	DBG("parsing data");
	if (pr.dataSize == -1)
		return std::nullopt;

	pr.dataBuffer.reserve(pr.dataSize);
	pr.dataBuffer.assign(rawData.begin(), rawData.begin() + pr.dataSize);
	return pr;
}

std::string messaging::ClientProtocol::constructRequest(
	std::string_view msg,
	std::string_view recver,
	ActionType requestType)
{
	// calculate total payload size
	const size_t payloadLength = REQUEST_HEADER_SIZE + msg.size() + recver.size();
	std::string header = constructHeader(msg.size() + recver.size(), requestType);
	std::string data = constructData(msg, recver);
	return header + data;
}

std::string messaging::ClientProtocol::constructHeader(size_t msgLength, ActionType requestType)
{
	// check for msg length
	if (msgLength > MAX_MESSAGE_LENGTH)
	{
		std::cerr << "Message length too long (max: " << MAX_MESSAGE_LENGTH << ")" << std::endl;
		return {};
	}

	// construct header
	char headerBuf[REQUEST_HEADER_SIZE + 1] = { 0 };
	sprintf_s(
		headerBuf,
		static_cast<int>(sizeof(headerBuf)),
		"%0*u%0*u%u",
		static_cast<int>(REQUEST_DATA_LENGTH_SIZE),
		static_cast<int>(msgLength),
		static_cast<int>(REQUEST_TYPE_SIZE),
		requestType,
		static_cast<int>(PROTOCOL_VERSION));

	DBG("constructed header: " << headerBuf);
	return std::string(headerBuf, REQUEST_HEADER_SIZE);
}

std::string messaging::ClientProtocol::constructData(std::string_view msg, std::string_view recver)
{
	if (recver.empty())
		return std::string(recver) + REQUEST_DATA_SEPERATOR + std::string(msg);
	return std::string(msg);
}

void messaging::ClientProtocol::extractLength(ParsedResponse& pr, std::string_view rawHeader)
{
	int intLength;
	char* endptr;

	// make a buffer the size of the length field in the header
	char cRawHeader[REQUEST_DATA_LENGTH_SIZE + 1] = { 0 };
	memcpy(cRawHeader, rawHeader.data(), REQUEST_DATA_LENGTH_SIZE);
	DBG("raw header length field: " << cRawHeader);

	// convert the length field to an integer
	intLength = strtol(cRawHeader, &endptr, 10);
	DBG("strtol result: " << intLength);

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


