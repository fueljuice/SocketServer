#include "ParsedRequest.h"
#include <iostream>
messaging::ParsedRequest::ParsedRequest()
	:
	dataSize(-1),
	requestType(INVALIDACTION),
	dataBuffer(nullptr)
	{}


// copy constuctor for std move

messaging::ParsedRequest::ParsedRequest(ParsedRequest&& other) noexcept
	:
	dataSize(other.dataSize),
	requestType(other.requestType),
	dataBuffer(other.dataBuffer),
	userName(other.userName)
{
	other.dataSize = -1;
	other.dataBuffer = nullptr;
	other.userName = nullptr;
	other.requestType = action::INVALIDACTION;
}

messaging::ParsedRequest::~ParsedRequest()
{
	delete[] dataBuffer;
	delete[] userName;
};