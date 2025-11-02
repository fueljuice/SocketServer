#pragma once
#include <iostream>
#include <stdlib.h>
#include <string>

//PROTOCOL:
// FIRST FOUR BYTES: INTEGER LENGTH OF THE DATA
// BYTE NUMBER 4-8:
// ENUM ACTION:
// 1: GETCHAT
// 2: SENDMESSAGE
// IF GETCHAT: NO BYTES ARE READ
// IF SENDMESSAGE: CONSIDER BYTE 8 - rawRequestLength THE MESSAGE
//
//

namespace messaging
{

	enum action : int
	{
		INVALIDACTION = -1,
		GETCHAT = 1,
		SENDMESSAGE = 2,

	};

	struct ParsedRequest
	{
		int dataSize;
		action requestType;
		unsigned int statusCode;
		char* databuffer;
	};

	class ParsingProtocol
	{
	private:
		const char* rawRequest;
		const int rawRequestLength;
		ParsedRequest pr;


		void extractLength();

		void extractRequestType();

		void extractData();

	public:
		ParsingProtocol(const char* rawbuf, int rawlength);

		ParsingProtocol(const char* lenbuf);

		int getRequestLength();

		ParsedRequest enforceProtocol();
	};
}