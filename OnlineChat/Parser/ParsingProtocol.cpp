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

		headerLength = 0;

		// could potentially store length header and request type header

		if(rawLength >= 2 * INTSIZE)
			headerLength = 2*INTSIZE;

		// if client only sent enough for the first header: theres no room for other headers
		if (rawLength == INTSIZE)
			headerLength = INTSIZE; 
	}

// a constructor overcload that gets a ParsedRequest and copies its data into the members.
// together with a new buff from the client, and its length
messaging::ParsingProtocol::ParsingProtocol(messaging::ParsedRequest& otherPr, const char* rawBuf, int rawLength)
	: 
	pr(std::move(otherPr)),
	rawRequest(rawBuf),
	rawRequestLength(rawLength)
	{
	pr.statusCode = 200;
	headerLength = 0;


	// if the data length is not initliazed (its equals 0) it means it the length wasnt
	// read from the buffer. so its still in it
	if (pr.dataSize == -1)
		headerLength += INTSIZE;

	// if the request type is not initliazed (its equals INVALIDACTION) it means it the length wasnt
	// read from the buffer. so its still in it
	if (pr.requestType == INVALIDACTION) // if theres not a request type its still in the header
		headerLength += INTSIZE;

	std::cout << "ctor of parsingProtocol called with parsed request" << std::endl;


	}

// extracts the length and returns it
int messaging::ParsingProtocol::getRequestLength()
{
	if(rawRequestLength == 4)
		extractLength();
	return pr.dataSize;
}

// extarcts all the data from the headers and the data itself into the ParsedRequest struct
messaging::ParsedRequest messaging::ParsingProtocol::enforceProtocol()
{
	std::cout << "enforcing protocol....  header length: " + headerLength << std::endl;
	std::cout << headerLength << std::endl;
	
	// extract the length only if it wasnt only already extracted
	if(pr.dataSize == -1)
		extractLength();

	// extract the request type only if it wasnt only extracted and if the raw buffer is big enough
	if(pr.requestType == INVALIDACTION && rawRequestLength >= 8)
		extractRequestType();

	// only if needed and if request type was suscsessful and theres data length
	if(pr.requestType == SENDMESSAGE && pr.dataSize > -1)
		extractData();
	
	// if request type isnt good make the status code 404 (error)
	if( static_cast<int>(pr.requestType) < 1 || static_cast<int>(pr.requestType) > 2)
	{
		std::cout << "BAD 404 IN THE PARSING" << std::endl;
		pr.statusCode = 404;
	}


	return std::move(pr);
}
// extracts length of the data from the header into the ParsingProtocol
void messaging::ParsingProtocol::extractLength()
{

	int length;
	char charLength[INTSIZE + 1] = {0};
	std::cout << "extracting length.." << std::endl;
	
	
	
	// reads for bytes and converts them into int
	memcpy(charLength, rawRequest, 4);
	std::cout << "after mem copy: " + std::string(charLength) << std::endl;
	length = atoi(charLength);


	// if read didnt sucsessfuly unitialize the data
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

// extracts request type from header to
void messaging::ParsingProtocol::extractRequestType()
{
	std::cout << "EXTRACTING REQUEST TYPE.." << std::endl;
	std::cout << "raw request: " + std::string(rawRequest) << std::endl;


	action reqType;
	char reqTypeChar[INTSIZE + 1] = {0};
	unsigned int reqTypeOffset = headerLength - INTSIZE;
	
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
	std::cout << "extracting data...." << std::endl;
	pr.databuffer = new char[pr.dataSize + headerLength];
	memcpy(pr.databuffer, rawRequest + headerLength, rawRequestLength - headerLength);
	printf("data parsed: %4s", pr.databuffer);

}
