#pragma once


#include <iostream>
#include <stdlib.h>
#include <string>
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

	static std::optional<ParsedRequest> parseHeader(const char* rawHeader, int rawLength);
	
	static ParsedRequest parseData(ParsedRequest&& pr, char* rawData);
	
	static bool isStatusOK(const ParsedRequest& pr, bool isRegistered);

	static std::pair<std::string, std::string> parseDirectMessage(std::string_view);

private:

	static void extractLength(ParsedRequest& pr, const char* rawHeader);

	static void extractRequestType(ParsedRequest& pr, const char* rawHeader);
	
	static void extractProtocolVersion(ParsedRequest& pr, const char* rawHeader);

	static void extractData(ParsedRequest& pr, const char* rawData);

	static bool isHeaderOK(const ParsedRequest& pr);

	static std::string constructResponseHeader(int length);




};
}
