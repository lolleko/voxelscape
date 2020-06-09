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
    int maxHeight = worldSize.y;
    float stepX = (float)(worldSize.x - 1) / (width - 1);
    float stepZ = (float)(worldSize.z - 1) / (height - 1);
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            for (int y = maxHeight - 1; y >= 0; y--)
            {
                int x = (int)std::round(i * stepX);
                int z = (int)std::round(j * stepZ);
                VSBlockID blockID = chunkManager->getBlock({x, y, z});
                if (blockID > 0)
                {
                    pixels.at(j * width * nrComponents + i * nrComponents) = blockID2MinimapColor.at(blockID).x;
                    pixels.at(j * width * nrComponents + i * nrComponents + 1) = blockID2MinimapColor.at(blockID).y;
                    pixels.at(j * width * nrComponents + i * nrComponents + 2) = blockID2MinimapColor.at(blockID).z;
                }
            }
        }
    }
    std::cout << "Updated minimap" << std::endl;
    // TODO Maybe check if out of world bounds if necessary
}