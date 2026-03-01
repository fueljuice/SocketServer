#include "ServerProtocol.h"
#include "ProtocolConstants.h"
#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG



std::string messaging::ServerProtocol::constructResponseHeader(size_t len)
{
	char formattedLength[messaging::RESPONSE_DATA_LENGTH_SIZE + 1] = { 0 };
	sprintf_s(
		formattedLength,
		static_cast<int>(sizeof(formattedLength)),
		"%0*d",
		static_cast<int>(messaging::RESPONSE_DATA_LENGTH_SIZE),
		static_cast<int>(len)); // formatting
	DBG("formatted length: " << formattedLength);
	return std::string(formattedLength, messaging::RESPONSE_DATA_LENGTH_SIZE);
}

std::string messaging::ServerProtocol::constructResponse(std::string payload)
{
	return constructResponseHeader(payload.size()) + payload;
}


// extracts header
std::optional<messaging::ParsedRequest> messaging::ServerProtocol::parseHeader(std::string rawHeader, size_t rawLength)
{
	DBG("parsing header");
	ParsedRequest pr;
	DBG("rawLength: " << rawLength);

	// header must be right length
	if (rawLength != messaging::REQUEST_HEADER_SIZE)
		return std::nullopt;

	// extractions
	extractLength(pr, rawHeader.c_str());
	extractRequestType(pr, rawHeader.c_str());
	extractProtocolVersion(pr, rawHeader.c_str());

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
	if (parsedRqst.requestType == ActionType::DIRECT_MESSAGE)
		parseDirectMessage(parsedRqst, rawData);

	// handle other types
	else if(parsedRqst.requestType != ActionType::INVALID)
		extractData(parsedRqst, rawData.c_str());

	return parsedRqst;
}

bool messaging::ServerProtocol::isStatusOK(const ParsedRequest& pr, bool isRegistered)
{
	// must have valid protocol version
	if (pr.protocolVersion != PROTOCOL_VERSION)
	{
		DBG("INVALID PROTOCOL VERSION: " << pr.protocolVersion);
		return false;
	}
	DBG("protocol version OK");
	DBG(static_cast<int>(pr.requestType) << ", " << pr.dataSize << ", " << pr.dataBuffer << ", " << isRegistered);
	// GET_CHAT
	if (pr.requestType == ActionType::GET_CHAT && pr.dataSize == 0) return true;// && isRegistered) return true; RETURN. ONLY FOR TESTING

	// SEND_MESSAGE
	else if (pr.requestType == ActionType::SEND_MESSAGE && pr.dataSize > 0 && !pr.dataBuffer.empty() && isRegistered) return true;

	// REGISTER
	else if (pr.requestType == ActionType::REGISTER && pr.dataSize > 0 && !pr.dataBuffer.empty() && !isRegistered) return true;

	// DIRECT MESSAGE
	else if (pr.requestType == ActionType::DIRECT_MESSAGE && pr.dataSize > 0 && !pr.dataBuffer.empty() && isRegistered) return true;

	return false;
}

bool messaging::ServerProtocol::isHeaderOK(const ParsedRequest& pr)
{
	return (pr.dataSize > -1 && pr.requestType != ActionType::INVALID && pr.protocolVersion == PROTOCOL_VERSION);
}

void messaging::ServerProtocol::parseDirectMessage(ParsedRequest& pr, std::string_view dmData)
{
	DBG("parsing direct message data: " << dmData);
	// search for data seperator
	std::size_t seperatorPos = dmData.find(messaging::REQUEST_DATA_SEPERATOR);
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
	ActionType reqType;
	char reqTypeChar[messaging::REQUEST_TYPE_SIZE + 1] = {0};
	
	// memcopies and atoi
	memcpy(reqTypeChar, rawHeader + reqTypeOffset, messaging::REQUEST_TYPE_SIZE);
	reqType = static_cast<ActionType>(atoi(reqTypeChar));

	if (static_cast<int>(reqType) == 0)
	{
		DBG("couldnt convert char length to int" );
		pr.requestType = ActionType::INVALID;
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
