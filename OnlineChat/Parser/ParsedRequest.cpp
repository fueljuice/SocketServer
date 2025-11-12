#include "ParsedRequest.h"

messaging::ParsedRequest::ParsedRequest()
	:
	dataSize(-1),
	requestType(INVALIDACTION),
	statusCode(404)
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
	other.dataSize = 0;
	other.databuffer = nullptr;
}

messaging::ParsedRequest::~ParsedRequest()
{
	if (databuffer != nullptr)
		delete databuffer;
};