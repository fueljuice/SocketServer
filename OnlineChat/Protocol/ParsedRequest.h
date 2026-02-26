#pragma once

#include "ProtocolConstants.h"
#include <string>

namespace messaging
{
struct ParsedRequest
{
	int dataSize;
	ActionType requestType;
	unsigned int protocolVersion;
	std::string dataBuffer;

	ParsedRequest();
	ParsedRequest(ParsedRequest&& other) noexcept = default;
	ParsedRequest(const ParsedRequest& other) noexcept = delete;
	~ParsedRequest() = default;
};

};

