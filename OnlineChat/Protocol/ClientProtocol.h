#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <cstdio>
#include <string_view>


#include "ProtocolConstants.h"
#include "ParsedResponse.h"
#include "../Client/ClientExceptions.h"
namespace messaging
{

	class ClientProtocol
	{

	public:
		static ParsedResponse parseHeader(std::string_view rawHeader, size_t rawLength);
		static ParsedResponse parseData(ParsedResponse&& pr, std::string rawData);
		static std::string constructRequest(std::string_view msg, std::string_view recver, ActionType requestType);

	private:
		static std::string constructHeader(size_t msgLength, ActionType requestType);
		static std::string constructData(std::string_view msg, std::string_view recver);
		// extarcts length of data	
		static void extractLength(ParsedResponse& pr, std::string_view rawHeader);
	};

}