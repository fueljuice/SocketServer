#pragma once

#include "ProtocolConstants.h"
#include <string>

namespace messaging
{
struct ParsedRequest
{
	int dataSize;
	ActionType requestType;
	std::string dataBuffer;
	std::string userName;

	ParsedRequest();
	ParsedRequest(ParsedRequest&& other) noexcept = default;
	ParsedRequest(const ParsedRequest& other) noexcept = delete;
	~ParsedRequest() = default;
};

};

