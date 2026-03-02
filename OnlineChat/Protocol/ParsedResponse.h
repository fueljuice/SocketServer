#pragma once
#include <string>

#include "ProtocolConstants.h"
namespace messaging
{
	struct ParsedResponse
	{
		ParsedResponse();
		int dataSize;
		ResponseCode responseCode;
		std::string dataBuffer;


	};
}

