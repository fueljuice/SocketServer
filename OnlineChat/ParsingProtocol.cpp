#include "ParsingProtocol.h"
#define  INTSIZE	4

messaging::ParsingProtocol::ParsingProtocol(const char* buf, int length)
	: 
	rawRequest(buf),
	rawRequestLength(length)
	{
		pr.statusCode = 200;
	}

messaging::ParsingProtocol::ParsingProtocol(const char* lenbuf)
	:rawRequest(lenbuf),
	rawRequestLength(4)
	{}

int messaging::ParsingProtocol::getRequestLength()
{
	return pr.dataSize;
}


messaging::ParsedRequest messaging::ParsingProtocol::enforceProtocol()
{
	if (!rawRequest || rawRequestLength < 4) 
	{
		std::cout << "buffer too short (need at least 8 bytes)\n";
		pr.statusCode = 404;
	}

	extractLength();
	extractRequestType();
	extractData();
	
	if( static_cast<int>(pr.requestType) < 1 || static_cast<int>(pr.requestType) > 2)
		pr.statusCode = 404;


	return std::move(pr);
}

void messaging::ParsingProtocol::extractLength()
{

	int length;
	char charLength[INTSIZE + 1] = {0};


	// according to the protocol, tries to read the first 4 bytes as the length
	memcpy(charLength, rawRequest, 4);
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
		std::cout << "length: " << pr.dataSize << std::endl;
	}

}

void messaging::ParsingProtocol::extractRequestType()
{
	action reqType;
	char reqTypeChar[INTSIZE + 1] = {0};


	memcpy(reqTypeChar, rawRequest + INTSIZE, INTSIZE);
	reqType = static_cast<action>(atoi(reqTypeChar));
	// if didnt read return false
	if (!reqType)
	{
		std::cout << "couldnt convert char length to int" << std::endl;
		pr.requestType = INVALIDACTION;
	}
	else
	{
		std::cout << "length: " << pr.dataSize << std::endl;
		pr.requestType = reqType;
	}

}

void messaging::ParsingProtocol::extractData()
{
	int subtract = INTSIZE + INTSIZE;
	memcpy(pr.databuffer, rawRequest + subtract, rawRequestLength - subtract);
}
