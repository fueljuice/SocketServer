#include "guiManager.h"

void GuiManager::logScreen(std::string_view msg, std::string_view errMsg)
{
    if (!errMsg.empty())
        std::cout << "\n[SYS Notification]:\n" << errMsg << std::endl;
    std::cout << msg << std::endl;
}
