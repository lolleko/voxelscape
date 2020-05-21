#pragma once

#include <glm/ext/vector_float3.hpp>

using VSBlockID = std::uint8_t;

constexpr std::uint8_t VS_DEFAULT_BLOCK_ID = 0;

struct VSBlockData
{
    glm::vec<3, std::byte> color;
};