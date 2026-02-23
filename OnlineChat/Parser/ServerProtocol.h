#pragma once


#include <iostream>
#include <stdlib.h>
#include <string>
#include <cstdio>
#include "parsedRequest.h"
#include <algorithm>

/*
 
-------PROTOCOL---------:

there are 2 types of requests getChat adn sendMessage.

getChat: returns the entire file content. doesnt need data and the datalength should be 0
and the request type shuld be 1

sendMessage: uploads a message into the file. the data should be the message itself and datalength should be 
the message length. and the request should be 2

the data length should be the first 0 - 4 bytes.

the request type should be from 4 - 8 bytes

the data should be the rest of the buffer.

UPDATE:

bytes 8 - 16 of header are username.


*/

namespace messaging
{
class ServerProtocol
{
public:

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
