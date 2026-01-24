#pragma once


#include <iostream>
#include <stdlib.h>
#include <string>
#include <cstdio>
#include "parsedRequest.h"


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



*/

namespace messaging
{
class ParsingProtocol
{
public:
	// constructs using a buffer and its lengtg
	ParsingProtocol(const char* rawBuf, int rawLength);
	// constructor that utilize data already gothered
	ParsingProtocol(messaging::ParsedRequest otherPr, const char* rawBuf, int rawLength);

	// returns a prasedRequest struct with the extarrcted header length
	ParsedRequest parseHeader();

	// returns a prasedRequest struct with the extarcted data 
	ParsedRequest parseData();

private:
	const char* rawRequest;
	const unsigned int rawRequestLength;
	ParsedRequest pr;
	// extarcts length of data	
	void extractLength();
	// extarcrts request type
	void extractRequestType();
	// extacrts data itself
	void extractData();

};
}