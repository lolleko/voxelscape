#pragma once

#include <cstdint>
#include <glm/vec3.hpp>
#include "game/components/unique.h"

struct Generator
{
    Unique resource;
    std::uint32_t amount;
    float interval;
    float lastGeneration;
};
