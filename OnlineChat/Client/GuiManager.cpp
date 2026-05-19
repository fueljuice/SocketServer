#include "guiManager.h"

void GuiManager::logScreen(std::string_view msg, std::string_view errMsg)
{
    if (!errMsg.empty())
        std::cout << "\n[Error]:\n" << errMsg << std::endl;
    std::cout << "\n[New]: \n" << msg << std::endl;
}
