#include "ParsedRequest.h"
#include <iostream>
#include <string>
#include <vector>

messaging::ParsedRequest::ParsedRequest()
	:
	dataSize(-1),
	requestType(ActionType::INVALID),
	protocolVersion(0),
	recver(std::nullopt)
	{}