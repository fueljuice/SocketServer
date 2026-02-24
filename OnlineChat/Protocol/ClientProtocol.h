#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <cstdio>
#include <array>
#include "ProtocolConstants.h"
#include "ParsedResponse.h"
#include "../Client/ClientExceptions.h"
namespace messaging
{

class ClientProtocol
{

public:
	static ParsedResponse parseHeader(const char* rawHeader, unsigned int rawLength);
	static ParsedResponse parseData(ParsedResponse&& pr, const char* rawData);
	static std::string constructRequest(unsigned int msgLength, const char* msg, unsigned int requestType, const char* name);

private:
	// extarcts length of data	
	static void extractLength(ParsedResponse& pr, const char* rawHeader, unsigned int rawLength);
};

}