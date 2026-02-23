#pragma once

namespace messaging
{

	enum action : int
	{
		INVALIDACTION = -1,
		GETCHAT = 1,
		SENDMESSAGE = 2,
		REGISTER = 3

	};

	struct ParsedRequest
	{
		int dataSize;
		action requestType;
		char* dataBuffer = { 0 };
		char* userName = { 0 };

		ParsedRequest();

		ParsedRequest(ParsedRequest&& other) noexcept;
		ParsedRequest(const ParsedRequest& other) noexcept = delete;
		~ParsedRequest();
	};

};

