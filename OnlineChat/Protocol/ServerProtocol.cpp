#include "ServerProtocol.h"
#include "ProtocolConstants.h"
#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG



std::string messaging::ServerProtocol::constructResponseHeader(int len)
{
	char formattedLength[messaging::RESPONSE_DATA_LENGTH_SIZE + 1] = { 0 };
	sprintf_s(formattedLength, sizeof(formattedLength), "%0*d", messaging::RESPONSE_DATA_LENGTH_SIZE, len); // formatting
	return std::string(formattedLength, messaging::RESPONSE_DATA_LENGTH_SIZE);
}

// extracts header
messaging::ParsedRequest messaging::ServerProtocol::parseHeader(const char* rawHeader, int rawLength)
{
	DBG("parsing header");
	ParsedRequest pr;
	DBG("rawLength: " << rawLength);
	if (rawLength != messaging::REQUEST_HEADER_SIZE)
		return pr;

	extractLength(pr, rawHeader);
	extractRequestType(pr, rawHeader);
	extractUserName(pr, rawHeader);

	return pr;
}

// extracts data
messaging::ParsedRequest messaging::ServerProtocol::parseData(ParsedRequest&& pr, char* rawData)
{
	DBG("parsing data");
	// check for valid conditions to extract data
	if (pr.requestType != ActionType::INVALID && pr.dataSize > 0)
		extractData(pr, rawData);
	return pr;
}

bool messaging::ServerProtocol::isStatusOK(const ParsedRequest& pr)
{
	// must have username
	if (pr.userName.empty())
		return false;
	DBG("username OK");

	// GET_CHAT
	if (pr.requestType == ActionType::GET_CHAT && pr.dataSize == 0) return true;

	// SEND_MESSAGE
	else if (pr.requestType == ActionType::SEND_MESSAGE && pr.dataSize > 0 && !pr.dataBuffer.empty()) return true;

	// REGISTER
	else if (pr.requestType == ActionType::REGISTER && pr.dataSize > 0 && !pr.dataBuffer.empty()) return true;

	return false;
}

bool messaging::ServerProtocol::isHeaderOK(const ParsedRequest& pr)
{
	return (!pr.userName.empty() && pr.dataSize > -1 && pr.requestType != ActionType::INVALID);
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
void messaging::ServerProtocol::extractUserName(ParsedRequest& pr, const char* rawHeader)
{
	constexpr unsigned int userNameOffset = messaging::USERNAME_OFFSET;

	DBG("extracting userName");
	// create string from raw header data
	pr.userName.assign(rawHeader + userNameOffset, messaging::USERNAME_SIZE);
	DBG("extracting userName done");

}


// extracts data into parsedrequest struct
void messaging::ServerProtocol::extractData(ParsedRequest& pr, const char* rawData)
{
	DBG("extracting data.... datasize: " << pr.dataSize);

	// create string from raw data
	pr.dataBuffer.assign(rawData, pr.dataSize);
}
