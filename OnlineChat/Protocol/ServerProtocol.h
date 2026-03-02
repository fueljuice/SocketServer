#pragma once


#include <iostream>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <cstdio>
#include "parsedRequest.h"
#include <algorithm>
#include <utility>
#include <optional>

#include "ParsedResponse.h"

namespace messaging
{
class ServerProtocol
{
public:


	static std::string constructResponse(std::string_view payload, ResponseCode code);
	static std::string constructResponse(ResponseCode code);

	static std::optional<ParsedRequest> parseHeader(std::string_view, size_t rawLength);
	
	static std::optional<ParsedRequest> parseData(ParsedRequest&& pr, std::string rawData);


private:
	static void extractDirectMessage(ParsedRequest& pr, std::string_view dmData);
	static void extractLength(ParsedRequest& pr, const char* rawHeader);
	static void extractRequestType(ParsedRequest& pr, const char* rawHeader);
	static void extractProtocolVersion(ParsedRequest& pr, const char* rawHeader);
	static void extractData(ParsedRequest& pr, const char* rawData);

	static bool isHeaderOK(const ParsedRequest& pr);

	static std::string constructResponseHeader(ResponseCode code, size_t dataLen);




};
}
