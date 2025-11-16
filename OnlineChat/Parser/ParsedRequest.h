#pragma once

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

		ParsedRequest();

		ParsedRequest(ParsedRequest&& other) noexcept;
		ParsedRequest(const ParsedRequest& other) noexcept = delete;
		~ParsedRequest();
	};

};

