#include "ParsingProtocol.h"
#define  INTSIZE	4



messaging::ParsingProtocol::ParsingProtocol(const char* rawBuf, int rawLength)
	: 
	rawRequest(rawBuf),
	rawRequestLength(rawLength)
	{
		std::cout << "ctor of parsingProtocol called with rawbuf and rawlength" << std::endl;

		pr.statusCode = 200;
		headerLength = INTSIZE + INTSIZE; // data length and request type
		if (rawLength == INTSIZE)
			headerLength = INTSIZE;
	}

messaging::ParsingProtocol::ParsingProtocol(messaging::ParsedRequest& otherPr, const char* rawBuf, int rawLength)
	: 
	pr(std::move(otherPr)),
	rawRequest(rawBuf),
	rawRequestLength(rawLength)
	{
	headerLength = 0;
	pr.statusCode = 200;
	if (pr.dataSize == 0)
		headerLength += INTSIZE; // if there is not length, its still in the header

	if (pr.requestType == INVALIDACTION) // if theres not a request type its still in the header
		headerLength += INTSIZE;

	std::cout << "ctor of parsingProtocol called with parsed request" << std::endl;


	}


int messaging::ParsingProtocol::getRequestLength()
{
	if(rawRequestLength == 4)
		extractLength();
	return pr.dataSize;
}


messaging::ParsedRequest messaging::ParsingProtocol::enforceProtocol()
{
	std::cout << "enforcing protocol....  header length: " + headerLength << std::endl;
	std::cout << headerLength << std::endl;
	if(pr.dataSize == 0)
		extractLength();

	if(pr.requestType == INVALIDACTION && rawRequestLength >= 8)
		extractRequestType();

	if(pr.requestType == SENDMESSAGE)
		extractData();
	
	if( static_cast<int>(pr.requestType) < 1 || static_cast<int>(pr.requestType) > 2)
	{
		std::cout << "BAD 404 IN THE PARSING" << std::endl;
		pr.statusCode = 404;
	}


	return std::move(pr);
}

void messaging::ParsingProtocol::extractLength()
{

	int length;
	char charLength[INTSIZE + 1] = {0};

	std::cout << "extracting length.." << std::endl;
	// according to the protocol, tries to read the first 4 bytes as the length
	const char* raw = rawRequest;
	std::cout << "raw request: " + std::string(rawRequest)  << std::endl;
	
	memcpy(charLength, rawRequest, 4);

	std::cout << "after mem copy: " + std::string(charLength) << std::endl;

	length = atoi(charLength);


	// if didnt read return false
	if (!length)
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

void messaging::ParsingProtocol::extractRequestType()
{
	std::cout << "EXTRACTING REQUEST TYPE.." << std::endl;
	std::cout << "raw request: " + std::string(rawRequest) << std::endl;
	action reqType;
	char reqTypeChar[INTSIZE + 1] = {0};
	unsigned int reqTypeOffset = headerLength - INTSIZE;

	memcpy(reqTypeChar, rawRequest + reqTypeOffset, INTSIZE);
	reqType = static_cast<action>(atoi(reqTypeChar));
	// if didnt read return false
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

void messaging::ParsingProtocol::extractData()
{
	std::cout << "extracting data...." << std::endl;
	pr.databuffer = new char[pr.dataSize + headerLength];
	memcpy(pr.databuffer, rawRequest + headerLength, rawRequestLength - headerLength);
	printf("data parsed: %4s", pr.databuffer);

}
