#include "ClientProtocol.h"
#include "ProtocolConstants.h"
#include "../Client/ClientExceptions.h"

#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

std::optional<messaging::ParsedResponse> messaging::ClientProtocol::parseHeader(
	std::string_view rawHeader,
	size_t rawLength)
{
	DBG("rawlength:" << rawLength);
	ParsedResponse prsdRqst;
	// header must be the correct length
	if (rawLength != RESPONSE_HEADER_SIZE)
		return std::nullopt;

	DBG("rawlength ok");
	// extract and validate length
	extractLength(prsdRqst, rawHeader);
	if (prsdRqst.dataSize < 0 || prsdRqst.dataSize > MAX_MESSAGE_LENGTH)
		return std::nullopt;
	DBG("extracting responseCode...");
	// extract and validate response code
	extractResponseCode(prsdRqst, rawHeader);
	if (prsdRqst.responseCode == ResponseCode::NO_RESPONSE)
		return std::nullopt;

	return prsdRqst;
}

std::optional <messaging::ParsedResponse> messaging::ClientProtocol::parseData(
	ParsedResponse&& prsdRqst,
	std::string rawData)
{
	DBG("parsing data");

	if (prsdRqst.dataSize == 0)
		return std::nullopt;

	prsdRqst.dataBuffer.reserve(prsdRqst.dataSize);
	prsdRqst.dataBuffer.assign(rawData.begin(), rawData.begin() + prsdRqst.dataSize);
	DBG("parsed data:" << prsdRqst.dataBuffer);
	return prsdRqst;
}

std::string messaging::ClientProtocol::constructRequest(
	std::string_view msg,
	std::string_view recver,
	RequestType requestType)
{
	// calculate total payload size
	std::string data = constructData(msg, recver);
	std::string header = constructHeader(data.size(), requestType);
	return header + data;
}

std::string messaging::ClientProtocol::constructHeader(size_t msgLength, RequestType requestType)
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
		static_cast<int>(requestType),
		static_cast<int>(PROTOCOL_VERSION));

	DBG("constructed header: " << headerBuf);
	return std::string(headerBuf, REQUEST_HEADER_SIZE);
}

std::string messaging::ClientProtocol::constructData(std::string_view msg, std::string_view recver)
{
	if (!recver.empty())
		return std::string(recver) + REQUEST_DATA_SEPERATOR + std::string(msg);
	return std::string(msg);
}

void messaging::ClientProtocol::extractLength(ParsedResponse& prsdRqst, std::string_view rawHeader)
{
	int intLength;
	char* endptr;

	// make a buffer the size of the length field in the header
	char cRawHeader[RESPONSE_HEADER_SIZE + 1] = { 0 };
	memcpy(cRawHeader, rawHeader.data() + RESPONSE_DATA_LENGTH_OFFSET, RESPONSE_DATA_LENGTH_SIZE);
	DBG("raw header length field: " << cRawHeader);

	// convert the length field to an integer
	intLength = strtol(cRawHeader, &endptr, 10);
	DBG("strtol result: " << intLength);

	// if the length header is invalid
	if (cRawHeader == endptr)
	{
		DBG("failed extract length from header ");
		prsdRqst.dataSize = -1;
		return;
	}

	// sucsessful extraction
	prsdRqst.dataSize = intLength;
	DBG("sucsess length extraction :" << intLength);
}

void messaging::ClientProtocol::extractResponseCode(
	ParsedResponse& prsdRqst,
	std::string_view rawHeader)
{
	char codeBuf[messaging::RESPONSE_CODE_SIZE + 1] = { 0 };
	memcpy(codeBuf, rawHeader.data() + messaging::RESPONSE_CODE_OFFSET, messaging::RESPONSE_CODE_SIZE);
	prsdRqst.responseCode = static_cast<ResponseCode>(atoi(codeBuf));
}


