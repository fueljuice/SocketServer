#include "ServerProtocol.h"
#include "ProtocolConstants.h"
#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG



std::string messaging::ServerProtocol::constructResponseHeader(ResponseCode code)
{
	char formattedLength[messaging::RESPONSE_DATA_LENGTH_SIZE + 1] = { 0 };
	sprintf_s(
		formattedLength,
		static_cast<int>(sizeof(formattedLength)),
		"%0*d%0*d",
		static_cast<int>(messaging::RESPONSE_DATA_LENGTH_SIZE),
		0,
		static_cast<int>(code));
	DBG("formatted length: " << formattedLength);
	return std::string(formattedLength, messaging::RESPONSE_DATA_LENGTH_SIZE);
}

std::string messaging::ServerProtocol::constructResponse(std::string_view payload, ResponseCode code)
{
	return constructResponseHeader(code) + payload.data();
}

std::string messaging::ServerProtocol::constructResponse(ResponseCode code)
{
	return constructResponseHeader(code);
}


// extracts header
std::optional<messaging::ParsedRequest> messaging::ServerProtocol::parseHeader(std::string_view rawHeader, size_t rawLength)
{
	DBG("parsing header");
	ParsedRequest pr;
	DBG("rawLength: " << rawLength);

	// header must be right length
	if (rawLength != messaging::REQUEST_HEADER_SIZE)
		return std::nullopt;

	// extractions
	extractLength(pr, rawHeader.data());
	extractRequestType(pr, rawHeader.data());
	extractProtocolVersion(pr, rawHeader.data());

	// validates header
	if(!isHeaderOK(pr))
		return std::nullopt;

	return pr;
}

// extracts data
std::optional<messaging::ParsedRequest> messaging::ServerProtocol::parseData(ParsedRequest&& parsedRqst, std::string rawData)
{
	DBG("parsing data");
	// must have data
	if(parsedRqst.dataSize <= 0)
		return std::nullopt;

	// handle direct message
	if (parsedRqst.requestType == RequestType::DIRECT_MESSAGE)
	{
		extractDirectMessage(parsedRqst, rawData);
		if (!parsedRqst.recver)
			return std::nullopt;
	}

	// handle other types
	else if(parsedRqst.requestType != RequestType::INVALID)
		extractData(parsedRqst, rawData.c_str());

	return parsedRqst;
}

bool messaging::ServerProtocol::isHeaderOK(const ParsedRequest& pr)
{
	return (pr.dataSize > -1 && pr.requestType != RequestType::INVALID && pr.protocolVersion == PROTOCOL_VERSION);
}

void messaging::ServerProtocol::extractDirectMessage(ParsedRequest& pr, std::string_view dmData)
{
	DBG("parsing direct message data: " << dmData);
	// search for data seperator
	const std::size_t seperatorPos = dmData.find(messaging::REQUEST_DATA_SEPERATOR);
	if (seperatorPos == std::string::npos || seperatorPos >= dmData.size() -1)
	{
		// no seperator
		pr.dataBuffer.clear();
		pr.recver = std::nullopt;
		return;
	}
	// sucsessful extraction
	pr.recver = std::string(dmData.substr(0, seperatorPos));
	pr.dataBuffer = std::string(dmData.substr(seperatorPos + 1));

}


// extracts length of the data from the header into the ServerProtocol
void messaging::ServerProtocol::extractLength(ParsedRequest& pr, const char* rawHeader)
{
	char* endptr;
	int length;
	char charLength[messaging::REQUEST_DATA_LENGTH_SIZE + 1] = {0};
	DBG("extracting length..");
	
	
	// reads for bytes and converts them into int
	memcpy(charLength, rawHeader, messaging::REQUEST_DATA_LENGTH_SIZE);
	DBG("after mem copy: " + std::string(charLength));
	length = strtol(charLength, &endptr, 10);


	// if read didnt sucsessfuly unitialize the data
	if (endptr == charLength)
	{
		DBG("couldnt convert char length to int");
		return;
	}

	pr.dataSize = length;
	DBG("int value of length: ");
	

}

// extracts request type from header to
void messaging::ServerProtocol::extractRequestType(ParsedRequest& pr, const char* rawHeader)
{
	constexpr unsigned int reqTypeOffset = messaging::REQUEST_TYPE_OFFSET;
	DBG("EXTRACTING REQUEST TYPE..");
	RequestType reqType;
	char reqTypeChar[messaging::REQUEST_TYPE_SIZE + 1] = {0};
	
	// memcopies and atoi
	memcpy(reqTypeChar, rawHeader + reqTypeOffset, messaging::REQUEST_TYPE_SIZE);
	reqType = static_cast<RequestType>(atoi(reqTypeChar));

	if (static_cast<int>(reqType) == 0)
	{
		DBG("couldnt convert char length to int" );
		pr.requestType = RequestType::INVALID;
	}
	else
	{
		DBG("reqType: " << static_cast<int>(reqType));
		pr.requestType = reqType;
	}

}



// extracts protocol version from header
void messaging::ServerProtocol::extractProtocolVersion(ParsedRequest& pr, const char* rawHeader)
{
	constexpr unsigned int versionOffset = messaging::PROTOCOL_VERSION_OFFSET;
	DBG("EXTRACTING PROTOCOL VERSION..");
	unsigned int version = static_cast<unsigned int>(rawHeader[versionOffset] - '0');
	pr.protocolVersion = version;
	DBG("protocol version: " << version);
}


// extracts data into parsedrequest struct
void messaging::ServerProtocol::extractData(ParsedRequest& pr, const char* rawData)
{
	// create string from raw data
	DBG("extracting data.... datasize: " << pr.dataSize);
	pr.dataBuffer.assign(rawData, pr.dataSize);
}
