#include "ParsedRequest.h"

messaging::ParsedRequest::ParsedRequest()
	:
	dataSize(-1),
	requestType(INVALIDACTION),
	statusCode(404),
	databuffer(nullptr)
{
}


// copy constuctor for std move

messaging::ParsedRequest::ParsedRequest(ParsedRequest&& other) noexcept
	:
	dataSize(other.dataSize),
	requestType(other.requestType),
	statusCode(other.statusCode),
	databuffer(other.databuffer)
{
	other.dataSize = -1;
	other.databuffer = nullptr;
}

messaging::ParsedRequest::~ParsedRequest()
{
	delete[] databuffer;
};