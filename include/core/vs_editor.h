#pragma once
#include "world/vs_world.h"

namespace VSEditor
{
    const std::string WorldName = "EDITOR";

    [[nodiscard]] VSWorld* initWorld();
};  // namespace VSEditor