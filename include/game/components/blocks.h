#pragma once

#include <glm/fwd.hpp>
#include <vector>

#include "world/vs_block.h"

struct Blocks
{
    std::vector<VSBlockID> blocks;
    glm::ivec3 size;
};