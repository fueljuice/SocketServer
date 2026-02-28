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
namespace messaging
{
class ServerProtocol
{
public:


	static std::string constructResponse(std::string payload);

	static std::optional<ParsedRequest> parseHeader(std::string, size_t rawLength);
	
	static std::optional<ParsedRequest> parseData(ParsedRequest&& pr, std::string rawData);
	
	static bool isStatusOK(const ParsedRequest& pr, bool isRegistered);


private:
	static void parseDirectMessage(ParsedRequest& pr, std::string_view dmData);

	static void extractLength(ParsedRequest& pr, const char* rawHeader);

	static void extractRequestType(ParsedRequest& pr, const char* rawHeader);
	
	static void extractProtocolVersion(ParsedRequest& pr, const char* rawHeader);

	static void extractData(ParsedRequest& pr, const char* rawData);

	static bool isHeaderOK(const ParsedRequest& pr);

	static std::string constructResponseHeader(size_t length);




};
}
