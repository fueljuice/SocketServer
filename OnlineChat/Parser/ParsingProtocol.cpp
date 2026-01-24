#include "ParsingProtocol.h"
#define  INTSIZE	4
#ifdef PR_DEBUG
#define DBG(X) std::cout << X << std::endl
#else
#define DBG(X)
#endif // PR_DEBUG

// a constructor overcload that construcrts with a brand new buffer and its length
messaging::ParsingProtocol::ParsingProtocol(const char* rawBuf, int rawLength)
	: 
	rawRequest(rawBuf),
	rawRequestLength(rawLength)
	{
	DBG("ctor of parsingProtocol called with rawbuf and rawlength");

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
	DBG("ctor of parsingProtocol called with parsed request" );


	}



// extracts header, data length and request type, into the parsedRequest struct member
messaging::ParsedRequest messaging::ParsingProtocol::parseHeader()
{

	// extract the length only if it wasnt only already extracted and if the buffer is big enough
	if (pr.dataSize == -1 && rawRequestLength >= INTSIZE)
		extractLength();

	// extract the request type only if it wasnt only extracted and if the raw buffer is big enough
	if (pr.requestType == INVALIDACTION && rawRequestLength >= 2 * INTSIZE)
		extractRequestType();

	// invalidtaes bad requests
	if (static_cast<int>(pr.requestType) < 1 || static_cast<int>(pr.requestType) > 2)
	{
		DBG("BAD 404 IN THE PARSING" );
		pr.statusCode = 404;
	}

	return std::move(pr);
}

// extracts data, only works if header is already initiallized
messaging::ParsedRequest messaging::ParsingProtocol::parseData()
{
	// check for valid conditions to extract data
	if (pr.requestType == SENDMESSAGE && pr.dataSize > -1)
		extractData();
	else
		DBG("must parse header first");


	return std::move(pr);
}


// extracts length of the data from the header into the ParsingProtocol
void messaging::ParsingProtocol::extractLength()
{
	char* endptr;
	int length;
	char charLength[INTSIZE + 1] = {0};
	DBG("extracting length..");
	
	
	
	// reads for bytes and converts them into int
	memcpy(charLength, rawRequest, INTSIZE);
	DBG("after mem copy: " + std::string(charLength));
	length = strtol(charLength, &endptr, 10);


	// if read didnt sucsessfuly unitialize the data
	if (endptr == charLength)
	{
		DBG("couldnt convert char length to int");
		pr.dataSize = -1;
	}
	else
	{
		pr.dataSize = length;
		DBG("int value of length: ");
	}

}

// extracts request type from header to
void messaging::ParsingProtocol::extractRequestType()
{
	DBG("EXTRACTING REQUEST TYPE..");
	DBG("raw request: " + std::string(rawRequest));


	action reqType;
	char reqTypeChar[INTSIZE + 1] = {0};
	unsigned int reqTypeOffset = INTSIZE;
	
	// memcopies and atoi
	memcpy(reqTypeChar, rawRequest + reqTypeOffset, INTSIZE);
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
// extracts data into parsedrequest struct
void messaging::ParsingProtocol::extractData()
{
	constexpr unsigned int HEADER_SIZE = 2 * INTSIZE; 
	int offset = 0;
	// checking if the header is inside the request and moving the offset accordingly
	if (pr.dataSize == rawRequestLength)
		// if no header, no offset for the data
		offset = 0; 

	else if (pr.dataSize + HEADER_SIZE == rawRequestLength)
		// if the header is inside the request, the offset is from the end of the header
		offset = HEADER_SIZE;

	else
	{
		// unkokn length, dont read anything
		DBG("invalid request length" );
		return;
	}

	DBG("extracting data.... datasize: " << pr.dataSize );

	//copying a new buffer
	pr.databuffer = new char[pr.dataSize + 1];
	memcpy(pr.databuffer, rawRequest + offset, pr.dataSize);
	pr.databuffer[pr.dataSize] = '\0';

}

