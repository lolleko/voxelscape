#pragma once

#include <cstdint>
#include <glm/vec3.hpp>
#include "game/components/resources.h"

struct Storage
{
    Resources resource;
    std::uint32_t amount;
};
