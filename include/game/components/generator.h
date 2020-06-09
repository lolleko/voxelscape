#pragma once

#include <cstdint>
#include <glm/vec3.hpp>
#include "game/resources.h"

struct Generator
{
    Resources resource;
    std::uint32_t amount;
    float interval;
    float lastGeneration;
};
