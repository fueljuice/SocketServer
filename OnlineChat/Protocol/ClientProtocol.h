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
		static ParsedResponse parseHeader(std::string rawHeader, unsigned int rawLength);
		static ParsedResponse parseData(ParsedResponse&& pr, std::string rawData);
		static std::string constructRequest(unsigned int msgLength, std::string msg, std::string recver, ActionType requestType);

	private:
		static std::string constructHeader(unsigned int msgLength, ActionType requestType);
		static std::string constructData(unsigned int msgLength, std::string msg, std::string recver, ActionType requestType);
		// extarcts length of data	
		static void extractLength(ParsedResponse& pr, std::string rawHeader, unsigned int rawLength);
	};

}