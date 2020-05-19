#pragma once

using VSBlockID = std::uint8_t;

constexpr std::uint8_t VS_DEFAULT_BLOCK_ID = 0;

struct VSBlockData
{
    glm::vec4 color;  // tODO maybe vec3
};