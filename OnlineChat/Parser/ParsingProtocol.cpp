#include "ParsingProtocol.h"
#define  INTSIZE	4



messaging::ParsingProtocol::ParsingProtocol(const char* rawBuf, int rawLength)
	: 
	rawRequest(rawBuf),
	rawRequestLength(rawLength)
	{
		std::cout << "ctor of parsingProtocol called" << std::endl;
		pr.statusCode = 200;
	}

// constructor for the case that already got length. this ctor merges the length
// and the buffer it got from the client so it could parse cleanly
messaging::ParsingProtocol::ParsingProtocol(const char* rawBuf, int rawLength, int msgLength)
	:
	rawRequestLength(rawLength + 4)
	{
		std::cout << "ctor of parsingProtocol called. merging type" << std::endl;
		const int strLenOflenBuffer = 4;
		char lenBuffer[strLenOflenBuffer + 1] = { 0 };
		char* mergedBuffer = new char[rawLength + strLenOflenBuffer];

		std::cout << "sprintf_s" << std::endl;
		sprintf_s(lenBuffer, "%04d", msgLength); // prase the length to four bytes (4 is 0004)
		printf("buf: %s and raw: %s\n", lenBuffer, rawRequest);
		// merging the length and client buffer msg
		std::cout << "memcpy" << std::endl;
		memcpy(mergedBuffer, lenBuffer, strLenOflenBuffer);
		memcpy(mergedBuffer + strLenOflenBuffer, rawBuf, strlen(rawBuf));
		rawRequest = mergedBuffer;
		pr.statusCode = 200;



	}


messaging::ParsingProtocol::ParsingProtocol(const char* lenbuf)
	:rawRequest(lenbuf),
	rawRequestLength(4)
	{
		pr.statusCode = 200;
		std::cout << "ctor of parsingprotocol called" << std::endl;
		printf("buf: %s and raw: %s\n", lenbuf, rawRequest);
	}


int messaging::ParsingProtocol::getRequestLength()
{
	if(rawRequestLength == 4)
		extractLength();
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
	std::cout << "raw request: " + std::string(rawRequest) << std::endl;
	
	memcpy(charLength, rawRequest, 4);

	std::cout << "after mem copy: " + std::string(charLength) << std::endl;

	length = atoi(charLength);
	std::cout << "length after atoi: " + length << std::endl;


	// if didnt read return false
	if (!length)
	{
		std::cout << "couldnt convert char length to int" << std::endl;
		pr.dataSize = -1;
	}
	else
	{
		pr.dataSize = length;
		std::cout << "length(pq): " << pr.dataSize << std::endl;
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
	int offset = INTSIZE + INTSIZE;
	memcpy(pr.databuffer, rawRequest + offset, rawRequestLength - offset);
}
