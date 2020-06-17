#include "core/vs_editor.h"
#include <algorithm>
#include <glm/ext/matrix_projection.hpp>
#include "core/vs_camera.h"
#include "core/vs_debug_draw.h"
#include "core/vs_input_handler.h"

#include "core/vs_cameracontroller.h"
#include "world/vs_chunk_manager.h"
#include "world/vs_skybox.h"
#include "world/vs_world.h"

#include "core/vs_app.h"
#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"

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

    void handleBlockPlacement(VSInputHandler* inputHandler, VSWorld* world, bool windowHovered)
    {
        if (windowHovered)
        {
            // We do not handle the right click
            inputHandler->handleRightClick();
        }
        
        glm::vec3 worldPosNear = world->getCameraController()->getCameraInWorldCoords();
        glm::vec3 worldPosFar = world->getCameraController()->getMouseFarInWorldCoords();

        const auto hitResult = world->getChunkManager()->lineTrace(worldPosNear, worldPosFar);

        if (hitResult.bHasHit)
        {
            glm::vec3 mouseInWorldCoords = hitResult.hitLocation;

            // For debugging
            world->getDebugDraw()->drawSphere(mouseInWorldCoords, 0.5F, {255, 0, 0});

            if (!inputHandler->isRightClickHandled())
            {
                // Check if block is placed in bounds
                if (!world->getChunkManager()->isLocationInBounds(mouseInWorldCoords))
                {
                    // do nothing
                    return;
                }
                // Not pretty oof
                world->getChunkManager()->setBlock(
                    mouseInWorldCoords, VSApp::getInstance()->getUI()->getState()->bSetBlockID + 1);
                inputHandler->handleRightClick();
            }
            else if (!inputHandler->isMiddleClickHandled())
            {
                // Check if block is placed in bounds
                if (!world->getChunkManager()->isLocationInBounds(mouseInWorldCoords))
                {
                    // do nothing
                    return;
                }
                mouseInWorldCoords -= 0.05F * hitResult.hitNormal;
                world->getChunkManager()->setBlock(mouseInWorldCoords, 0);
                inputHandler->handleMiddleClick();
            }
        }
    }

    // Set blocks for a plane, should be called with the world as parameter that was returned by
    // initWorld() after the chunks were initialized
    void setPlaneBlocks(VSWorld* editorWorld)
    {
        const auto worldSize = editorWorld->getChunkManager()->getWorldSize();
        const auto worldSizeHalf = worldSize / 2;
        for (int x = -worldSizeHalf.x; x < worldSizeHalf.x; x++)
        {
            for (int z = -worldSizeHalf.z; z < worldSizeHalf.z; z++)
            {
                editorWorld->getChunkManager()->setBlock({x, -worldSizeHalf.y, z}, 1);
            }
        }
    }

    VSChunkManager::VSBuildingData extractBuildFromPlane(VSWorld* editorWorld)
    {
        VSChunkManager::VSBuildingData buildData;
        const auto worldSize = editorWorld->getChunkManager()->getWorldSize();
        const auto worldSizeHalf = worldSize / 2;
        int xMin = worldSize.x;
        int xMax = -worldSize.x;
        int yMin = worldSize.y;
        int yMax = -worldSize.y;
        int zMin = worldSize.z;
        int zMax = -worldSize.z;

        for (int x = -worldSizeHalf.x; x < worldSizeHalf.x; x++)
        {
            for (int z = -worldSizeHalf.z; z < worldSizeHalf.z; z++)
            {
                for (int y = -worldSizeHalf.y; y < worldSizeHalf.y; y++)
                {
                    int blockID = editorWorld->getChunkManager()->getBlock({x, y, z});
                    if (blockID > 1 || (blockID != 0 && y != -worldSizeHalf.y))
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