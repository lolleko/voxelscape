#pragma once

#include <cstdint>
#include "game/components/unique.h"

struct Cost
{
    Unique resource;
    std::uint32_t amount;
};
