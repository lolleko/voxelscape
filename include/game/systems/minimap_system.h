#include <cmath>
#include <entt/entt.hpp>
#include "core/vs_log.h"
#include "game/components/minimap.h"
#include "game/components/ui_context.h"
#include "game/components/world_context.h"
#include "world/vs_block.h"
#include "world/vs_chunk_manager.h"

void updateMinimapSystem(entt::registry& mainRegistry)
{
    auto& uiContext = mainRegistry.ctx<UIContext>();
    auto& minimap = uiContext.minimap;

    const auto& worldContext = mainRegistry.ctx<WorldContext>();

    if (minimap.bShouldUpdate)
    {
        minimap.pixels.resize(minimap.width * minimap.height * minimap.nrComponents);
        std::fill(minimap.pixels.begin(), minimap.pixels.end(), 0);

        auto* chunkManager = worldContext.world->getChunkManager();
        glm::ivec3 worldSize = chunkManager->getWorldSize();
        glm::ivec3 worldSizeHalf = worldSize / 2;
        float stepX = (float)(worldSize.x - 1) / minimap.width;
        float stepZ = (float)(worldSize.z - 1) / minimap.height;
        for (int i = -minimap.width / 2; i < minimap.width / 2; i++)
        {
            for (int j = -minimap.height / 2; j < minimap.height / 2; j++)
            {
                for (int y = worldSizeHalf.y - 1; y >= -worldSizeHalf.y; y--)
                {
                    int x = (int)std::round(i * stepX);
                    int z = (int)std::round(j * stepZ);
                    VSBlockID blockID = chunkManager->getBlock({x, y, z});
                    if (blockID > 0)
                    {
                        minimap.pixels.at(
                            (j + minimap.height / 2) * minimap.width * minimap.nrComponents +
                            (i + minimap.width / 2) * minimap.nrComponents) =
                            minimap.blockID2MinimapColor.at(blockID).x;
                        minimap.pixels.at(
                            (j + minimap.height / 2) * minimap.width * minimap.nrComponents +
                            (i + minimap.width / 2) * minimap.nrComponents + 1) =
                            minimap.blockID2MinimapColor.at(blockID).y;
                        minimap.pixels.at(
                            (j + minimap.height / 2) * minimap.width * minimap.nrComponents +
                            (i + minimap.width / 2) * minimap.nrComponents + 2) =
                            minimap.blockID2MinimapColor.at(blockID).z;
                        // TODO: Are breaks still cool? Otherwise will do a while loop
                        break;
                    }
                }
            }
        }
        minimap.bShouldUpdate = false;
        minimap.bIsUpdateCompleted = true;
    }

    if (minimap.bWasClicked)
    {
        float relativeX = minimap.relativeClickPosition.x;
        float relativeZ = minimap.relativeClickPosition.y;
        worldContext.world->getCameraController()->setCameraRelativeXZ(relativeX, relativeZ);
        minimap.bWasClicked = false;
    }
}