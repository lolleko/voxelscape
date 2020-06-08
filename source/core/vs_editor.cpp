#include "core/vs_editor.h"
#include <algorithm>
#include "core/vs_camera.h"
#include "world/vs_chunk_manager.h"
#include "world/vs_skybox.h"
#include "world/vs_world.h"

namespace VSEditor
{
    VSWorld* initWorld()
    {
        VSWorld* editorWorld = new VSWorld();
        auto skybox = new VSSkybox();
        editorWorld->addDrawable(skybox);
        editorWorld->getCamera()->setPosition(glm::vec3(-50.F, -5.F, -50.F));
        editorWorld->getCamera()->setPitchYaw(-10.F, 45.F);
        return editorWorld;
    }

    // Set blocks for a plane, should be called with the world as parameter that was returned by
    // initWorld() after the chunks were initialized
    void setPlaneBlocks(VSWorld* editorWorld)
    {
        const auto worldSize = editorWorld->getChunkManager()->getWorldSize();
        for (int x = 0; x < worldSize.x; x++)
        {
            for (int z = 0; z < worldSize.z; z++)
            {
                editorWorld->getChunkManager()->setBlock({x, 0, z}, 1);
            }
        }
    }

    VSChunkManager::VSBuildingData extractBuildFromPlane(VSWorld* editorWorld)
    {
        VSChunkManager::VSBuildingData buildData;
        const auto worldSize = editorWorld->getChunkManager()->getWorldSize();
        int xMin = worldSize.x;
        int xMax = -worldSize.x;
        int yMin = worldSize.y;
        int yMax = -worldSize.y;
        int zMin = worldSize.z;
        int zMax = -worldSize.z;

        for (int x = 0; x < worldSize.x; x++)
        {
            for (int z = 0; z < worldSize.z; z++)
            {
                for (int y = 0; y < worldSize.y; y++)
                {
                    int blockID = editorWorld->getChunkManager()->getBlock({x, y, z});
                    if (blockID > 1 || (blockID != 0 && y != 0))
                    {
                        xMin = std::min(xMin, x);
                        yMin = std::min(yMin, y);
                        zMin = std::min(zMin, z);
                        xMax = std::max(xMax, x);
                        yMax = std::max(yMax, y);
                        zMax = std::max(zMax, z);
                    }
                }
            }
        }

        buildData.buildSize = {
            std::abs(xMax - xMin) + 1, std::abs(yMax - yMin) + 1, std::abs(zMax - zMin) + 1};

        for (int x = xMin; x <= xMax; x++)
        {
            for (int y = yMin; y <= yMax; y++)
            {
                for (int z = zMin; z <= zMax; z++)
                {
                    buildData.blocks.emplace_back(
                        editorWorld->getChunkManager()->getBlock({x, y, z}));
                }
            }
        }

        return buildData;
    }
};  // namespace VSEditor