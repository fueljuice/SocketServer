#include "ParsingProtocol.h"
#define  INTSIZE	4
# define HEADER_SIZE	16
#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG



// extracts header
messaging::ParsedRequest messaging::ParsingProtocol::parseHeader(const char* rawHeader, int rawLength)
{
	DBG("parsing header");
	ParsedRequest pr;
	DBG("rawLength: " << rawLength);
	if (rawLength != HEADER_SIZE)
		return pr;

	extractLength(pr, rawHeader);
	extractRequestType(pr, rawHeader);
	extractUserName(pr, rawHeader);

	return pr;
}

// extracts data
messaging::ParsedRequest messaging::ParsingProtocol::parseData(ParsedRequest&& pr, char* rawData)
{
	DBG("parsing data");
	// check for valid conditions to extract data
	if (pr.requestType != INVALIDACTION && pr.dataSize > 0)
		extractData(pr, rawData);
	return pr;
}

bool messaging::ParsingProtocol::isStatusOK(const ParsedRequest& pr)
{
	// must have username
	if (pr.userName == nullptr)
		return false;
	DBG("username OK");

	// GETCHAT
	if (pr.requestType == action::GETCHAT && pr.dataSize == 0) return true;

	// SENDMESSAGE
	else if (pr.requestType == action::SENDMESSAGE && pr.dataSize > 0 && pr.dataBuffer != nullptr) return true;

	// REGISTERATE
	else if (pr.requestType == action::REGISTER && pr.dataSize > 0 && pr.dataBuffer != nullptr) return true;

	return false;
}

bool messaging::ParsingProtocol::isHeaderOK(const ParsedRequest& pr)
{
	return (pr.userName != nullptr && pr.dataSize > -1 && pr.requestType != action::INVALIDACTION);
}


// extracts length of the data from the header into the ParsingProtocol
void messaging::ParsingProtocol::extractLength(ParsedRequest& pr, const char* rawHeader)
{
	char* endptr;
	int length;
	char charLength[INTSIZE + 1] = {0};
	DBG("extracting length..");
	
	
	
	// reads for bytes and converts them into int
	memcpy(charLength, rawHeader, INTSIZE);
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
void messaging::ParsingProtocol::extractRequestType(ParsedRequest& pr, const char* rawHeader)
{
	constexpr unsigned int reqTypeOffset = INTSIZE;
	DBG("EXTRACTING REQUEST TYPE..");
	action reqType;
	char reqTypeChar[INTSIZE + 1] = {0};
	
	// memcopies and atoi
	memcpy(reqTypeChar, rawHeader + reqTypeOffset, INTSIZE);
	reqType = static_cast<action>(atoi(reqTypeChar));


	if (!reqType)
	{
		DBG("couldnt convert char length to int" );
		pr.requestType = INVALIDACTION;
	}
	else
	{
		DBG("reqType: " << reqType);
		pr.requestType = reqType;
	}

}
void messaging::ParsingProtocol::extractUserName(ParsedRequest& pr, const char* rawHeader)
{
	constexpr unsigned int userNameOffset = 2*INTSIZE;

	DBG("extracting userName");
	// creating data buffer
	pr.userName = new char[2*INTSIZE + 1];
	pr.userName[2*INTSIZE] = '\0';
	// copying
	memcpy(pr.userName, rawHeader + userNameOffset, 2*INTSIZE);
}


// extracts data into parsedrequest struct
void messaging::ParsingProtocol::extractData(ParsedRequest& pr, const char* rawData)
{
	DBG("extracting data.... datasize: " << pr.dataSize);

	// creating data buffer
	pr.dataBuffer = new char[pr.dataSize + 1];
	pr.dataBuffer[pr.dataSize] = '\0';
	// copying
	memcpy(pr.dataBuffer, rawData, pr.dataSize);


}

