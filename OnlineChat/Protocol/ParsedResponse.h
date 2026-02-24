#pragma once
#include <string>

namespace messaging
{
struct ParsedResponse
{
	ParsedResponse();
	unsigned int dataSize;
	std::string dataBuffer;


};
}

