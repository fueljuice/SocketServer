#pragma once
#include <string>
#include <iostream>
#include <unordered_map>
#include <optional>
#include "../Protocol/ProtocolConstants.h"
struct IGuiManager
{
public:
	virtual ~IGuiManager() = default;
	virtual void logScreen(std::string msg, messaging::ResponseCode code) = 0;
};

class GuiManager : public IGuiManager
{
public:
	void logScreen(std::string msg, messaging::ResponseCode code);

private:
	std::optional<std::string> messageForCode(messaging::ResponseCode code);
};

