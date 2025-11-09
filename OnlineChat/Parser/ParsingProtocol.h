#pragma once
#include <iostream>
#include <stdlib.h>
#include <string>
#include <cstdio>
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
		char* databuffer = { 0 };

		ParsedRequest()
			:
			dataSize(0),
			requestType(INVALIDACTION),
			statusCode(404)
		{}

		ParsedRequest(ParsedRequest&& other) noexcept
			: 
			dataSize(other.dataSize),
			requestType(other.requestType),
			statusCode(other.statusCode),
			databuffer(other.databuffer)
		{
			other.dataSize = 0;
			other.databuffer = nullptr;
		}

		~ParsedRequest()
		{
			if (databuffer != nullptr)
				delete databuffer;
		};
	};

	class ParsingProtocol
	{
	private:
		const char* rawRequest;
		const unsigned int rawRequestLength;
		ParsedRequest pr;
		unsigned int headerLength;
		


		void extractLength();

		void extractRequestType();

		void extractData();

	public:
		ParsingProtocol(const char* rawBuf, int rawLength);

		ParsingProtocol(messaging::ParsedRequest& otherPr, const char* rawBuf, int rawLength);

		int getRequestLength();

		ParsedRequest enforceProtocol();
	};
}