#pragma once
#include <string>

class VSWorld;

namespace VSMenu
{
    const std::string WorldName = "MENU";

    [[nodiscard]] VSWorld* initWorld();
}  // namespace VSMenu
