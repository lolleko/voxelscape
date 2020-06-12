#include "ui/vs_minimap.h"
#include <cmath>
#include "core/vs_log.h"
#include "renderer/vs_textureloader.h"
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

bool VSMinimap::hasChanged() const
{
    return dataChanged;
}

void VSMinimap::changeNotified()
{
    dataChanged = false;
}

void VSMinimap::updateMinimap(const VSWorld* world)
{
    auto chunkManager = world->getChunkManager();
    glm::ivec3 worldSize = chunkManager->getWorldSize();
    glm::ivec3 worldSizeHalf = worldSize / 2;
    float stepX = (float)(worldSize.x - 1) / width;
    float stepZ = (float)(worldSize.z - 1) / height;
    for (int i = -width / 2; i < width / 2; i++)
    {
        for (int j = -height / 2; j < height / 2; j++)
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
                    // TODO: Are breaks still cool? Otherwise will do a while loop
                    break;
                }
            }
        }
    }
    dataChanged = true;
}