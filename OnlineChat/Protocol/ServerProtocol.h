#pragma once


#include <iostream>
#include <stdlib.h>
#include <string>
#include <cstdio>
#include "parsedRequest.h"
#include <algorithm>

/*
 
-------PROTOCOL---------:
header: 
	0-3: data length
	4-7: request type
	8-15: username
------------------------

*/

namespace messaging
{
class ServerProtocol
{
public:

	// constructs the response header
	static std::string constructResponseHeader(int length);
	// returns a prasedRequest struct with the extarrcted header length
	static ParsedRequest parseHeader(const char* rawHeader, int rawLength);
	// returns a prasedRequest struct with the extarcted data 
	static ParsedRequest parseData(ParsedRequest&& pr, char* rawData);
	// checks wether the request is valid
	static bool isStatusOK(const ParsedRequest& pr);
	// checks if header is ok
	static bool isHeaderOK(const ParsedRequest& pr);

private:
	// extarcts length of data	
	static void extractLength(ParsedRequest& pr, const char* rawHeader);
	// extarcrts request type
	static void extractRequestType(ParsedRequest& pr, const char* rawHeader);
	// extracts userName
	static void extractUserName(ParsedRequest& pr, const char* rawHeader);
	// extacrts data itself
	static void extractData(ParsedRequest& pr, const char* rawData);

};
}
