#include "ui/vs_minimap.h"
#include <algorithm>
#include <cmath>
#include <glm/fwd.hpp>
#include "core/vs_log.h"
#include "world/vs_block.h"
#include "world/vs_chunk_manager.h"

VSMinimap::VSMinimap()
{
    pixels.resize(width * height * nrComponents);
    std::fill(pixels.begin(), pixels.end(), 0);
}

int VSMinimap::getWidth()
{
    return width;
}

int VSMinimap::getHeight()
{
    return height;
}

int VSMinimap::getNrComponents()
{
    return nrComponents;
}

unsigned char* VSMinimap::getPixelData()
{
    return pixels.data();
}

void VSMinimap::updateMinimap(const VSWorld* world)
{
    auto chunkManager = world->getChunkManager();
    glm::ivec3 worldSize = chunkManager->getWorldSize();
    glm::ivec3 worldSizeHalf = worldSize / 2;
    float stepX = (float)(worldSize.x - 1) / (width - 1);
    float stepZ = (float)(worldSize.z - 1) / (height - 1);
    for (int i = -worldSizeHalf.x; i < worldSizeHalf.x; i++)
    {
        for (int j = -worldSizeHalf.z; j < worldSizeHalf.z; j++)
        {
            for (int y = worldSizeHalf.y - 1; y >= -worldSizeHalf.y; y--)
            {
                int x = (int)std::round(i * stepX);
                int z = (int)std::round(j * stepZ);
                VSBlockID blockID = chunkManager->getBlock({x, y, z});
                if (blockID > 0)
                {
                    pixels.at((j + height / 2) * width * nrComponents + (i + width / 2) * nrComponents) = blockID2MinimapColor.at(blockID).x;
                    pixels.at((j + height / 2) * width * nrComponents + (i + width / 2) * nrComponents + 1) = blockID2MinimapColor.at(blockID).y;
                    pixels.at((j + height / 2) * width * nrComponents + (i + width / 2) * nrComponents + 2) = blockID2MinimapColor.at(blockID).z;
                }
            }
        }
    }
}