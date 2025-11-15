#include "ParsingProtocol.h"
#define  INTSIZE	4


// a constructor overcload that construcrts with a brand new buffer and its length
messaging::ParsingProtocol::ParsingProtocol(const char* rawBuf, int rawLength)
	: 
	rawRequest(rawBuf),
	rawRequestLength(rawLength)
	{
		std::cout << "ctor of parsingProtocol called with rawbuf and rawlength" << std::endl;

		pr.statusCode = 200;
	}

// a constructor overcload that gets a ParsedRequest and copies its data into the members.
// together with a new buff from the client, and its length
messaging::ParsingProtocol::ParsingProtocol(messaging::ParsedRequest otherPr, const char* rawBuf, int rawLength)
	: 
	pr(std::move(otherPr)),
	rawRequest(rawBuf),
	rawRequestLength(rawLength)
	{
	pr.statusCode = 200;

	std::cout << "ctor of parsingProtocol called with parsed request" << std::endl;


	}

// extracts the length and returns it
int messaging::ParsingProtocol::getRequestLength()
{
	if(rawRequestLength == 4)
		extractLength();
	return pr.dataSize;
}

// extracts header, data length and request type, into the parsedRequest struct member
messaging::ParsedRequest messaging::ParsingProtocol::parseHeader()
{

	// extract the length only if it wasnt only already extracted
	if (pr.dataSize == -1)
		extractLength();

	// extract the request type only if it wasnt only extracted and if the raw buffer is big enough
	if (pr.requestType == INVALIDACTION && rawRequestLength >= 8)
		extractRequestType();

	// invalidtaes bad requests
	if (static_cast<int>(pr.requestType) < 1 || static_cast<int>(pr.requestType) > 2)
	{
		std::cout << "BAD 404 IN THE PARSING" << std::endl;
		pr.statusCode = 404;
	}

	return std::move(pr);
}

// extracts data, only works if header is already initiallized
messaging::ParsedRequest messaging::ParsingProtocol::parseData()
{

	if (pr.requestType == SENDMESSAGE && pr.dataSize > -1)
		extractData();
	else
		std::cout << "must parse header first";


	return std::move(pr);
}


// extracts length of the data from the header into the ParsingProtocol
void messaging::ParsingProtocol::extractLength()
{
	char* endptr;
	int length;
	char charLength[INTSIZE + 1] = {0};
	std::cout << "extracting length.." << std::endl;
	
	
	
	// reads for bytes and converts them into int
	memcpy(charLength, rawRequest, 4);
	std::cout << "after mem copy: " + std::string(charLength) << std::endl;
	length = strtol(charLength, &endptr, 10);


	// if read didnt sucsessfuly unitialize the data
	if (endptr == charLength)
	{
		std::cout << "couldnt convert char length to int" << std::endl;
		pr.dataSize = -1;
	}
	else
	{
		pr.dataSize = length;
		std::cout << "int value of length: " << pr.dataSize << std::endl;
	}

}

// extracts request type from header to
void messaging::ParsingProtocol::extractRequestType()
{
	std::cout << "EXTRACTING REQUEST TYPE.." << std::endl;
	std::cout << "raw request: " + std::string(rawRequest) << std::endl;


	action reqType;
	char reqTypeChar[INTSIZE + 1] = {0};
	unsigned int reqTypeOffset = INTSIZE;
	
	// memcopies and atoi
	memcpy(reqTypeChar, rawRequest + reqTypeOffset, INTSIZE);
	reqType = static_cast<action>(atoi(reqTypeChar));


	if (!reqType)
	{
		std::cout << "couldnt convert char length to int" << std::endl;
		pr.requestType = INVALIDACTION;
	}
	else
	{
		std::cout << "reqType: " << reqType << std::endl;
		pr.requestType = reqType;
	}

}
// extracts data into parsedrequest struct
void messaging::ParsingProtocol::extractData()
{
	std::cout << "extracting data.... datasize: " << pr.dataSize << std::endl;
	pr.databuffer = new char[pr.dataSize + 1];
	memcpy(pr.databuffer, rawRequest, pr.dataSize);
	pr.databuffer[pr.dataSize] = '\0';
	printf("data parsed: %4s\n", pr.databuffer);

}

