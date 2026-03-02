#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <cstdio>
#include <string_view>
#include <optional>

#include "ProtocolConstants.h"
#include "ParsedResponse.h"
#include "../Client/ClientExceptions.h"
namespace messaging
{

	class ClientProtocol
	{

	public:
		static std::optional<ParsedResponse> parseHeader(std::string_view rawHeader, size_t rawLength);
		static std::optional<ParsedResponse> parseData(ParsedResponse&& prsdRqst, std::string rawData);
		static std::string constructRequest(std::string_view msg, std::string_view recver, RequestType requestType);

	private:
		static std::string constructHeader(size_t msgLength, RequestType requestType);
		static std::string constructData(std::string_view msg, std::string_view recver);

		static void extractLength(ParsedResponse& prsdRqst, std::string_view rawHeader);
		static void extractResponseCode(ParsedResponse& prsdRqst, std::string_view rawHeader);
	};

}