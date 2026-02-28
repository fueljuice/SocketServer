#pragma once
#include <optional>
#include <utility>
#include "ProtocolConstants.h"
#include <string>

namespace messaging
{
struct ParsedRequest
{
	std::optional<std::string> recver;
	std::string dataBuffer;
	unsigned int dataSize;
	ActionType requestType;
	unsigned int protocolVersion;

	ParsedRequest();
	ParsedRequest(ParsedRequest&& other) noexcept = default;
	ParsedRequest(const ParsedRequest& other) noexcept = delete;
	~ParsedRequest() = default;
};

};

