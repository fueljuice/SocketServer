#pragma once
#include <string>

namespace messaging
{
struct ParsedResponse
{
	ParsedResponse();
	int dataSize;
	std::string dataBuffer;


};
}

