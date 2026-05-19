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
	virtual void logScreen(std::string_view msg, std::string_view errMsg) = 0;
};

class GuiManager : public IGuiManager
{
public:
	void logScreen(std::string_view msg, std::string_view errMsg);

};

