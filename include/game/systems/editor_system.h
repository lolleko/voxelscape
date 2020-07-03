#pragma once

#include <entt/entt.hpp>
#include "game/components/inputs.h"
#include "game/components/ui_context.h"
#include "game/components/world_context.h"

void updateEditorSystem(entt::registry& mainRegistry)
{
    const auto& inputs = mainRegistry.ctx<Inputs>();

    const auto& worldContext = mainRegistry.ctx<WorldContext>();

    const auto& uiContext = mainRegistry.ctx<UIContext>();

    if (inputs.mouseTrace.bHasHit)
    {
        const auto mouseLocation = inputs.mouseTrace.hitLocation;
        // Check if block is placed in bounds
        if (!worldContext.bounds.isLocationInside(mouseLocation))
        {
            return;
        }
        // For debugging
        worldContext.world->getDebugDraw()->drawSphere(mouseLocation, 0.5F, {255, 0, 0});

        if (inputs.rightButtonState == InputState::JustUp)
        {
            worldContext.world->getChunkManager()->setBlock(
                mouseLocation, uiContext.editorSelectedBlockID + 1);
        }
        else if (inputs.middleButtonState == InputState::JustUp)
        {
            worldContext.world->getChunkManager()->setBlock(
                mouseLocation - 0.05F * inputs.mousTrace.hitNormal, 0);
        }
    }
}

// if (uiContext.bShouldResetEditor && !world->getChunkManager()->shouldReinitializeChunks())
// {
//     // TODO: Clear world
//     // world->getChunkManager()->clearBlocks();
//     VSEditor::setPlaneBlocks(world);
//     uiContext.bShouldResetEditor = false;
// }

// if (uiContext.bShouldSaveToFile)
//     {
//         VSChunkManager::VSWorldData worldData = world->getChunkManager()->getData();
//         VSParser::writeToFile(worldData, uiContext.saveFilePath);
//         uiContext.bShouldSaveToFile = false;
//     }

//     if (uiContext.bShouldSaveBuilding)
//     {
//         // Extract editor plane and save building
//         VSChunkManager::VSBuildingData buildData = VSEditor::extractBuildFromPlane(world);
//         VSParser::writeBuildToFile(buildData, uiContext.saveBuildingPath);
//         uiContext.bShouldSaveBuilding = false;
//     }

//     if (uiContext.bShouldLoadFromFile)
//     {
//         VSChunkManager::VSWorldData worldData = VSParser::readFromFile(uiContext.loadFilePath);
//         world->getChunkManager()->setWorldData(worldData);
//         uiContext.bShouldLoadFromFile = false;
//     }

// // Set blocks for a plane, should be called with the world as parameter that was returned by
// // initWorld() after the chunks were initialized
// void setPlaneBlocks(VSWorld* editorWorld)
// {
//     const auto worldSize = editorWorld->getChunkManager()->getWorldSize();
//     const auto worldSizeHalf = worldSize / 2;
//     for (int x = -worldSizeHalf.x; x < worldSizeHalf.x; x++)
//     {
//         for (int z = -worldSizeHalf.z; z < worldSizeHalf.z; z++)
//         {
//             editorWorld->getChunkManager()->setBlock({x, -worldSizeHalf.y, z}, 1);
//         }
//     }
// }

// VSChunkManager::VSBuildingData extractBuildFromPlane(VSWorld* editorWorld)
// {
//     VSChunkManager::VSBuildingData buildData;
//     const auto worldSize = editorWorld->getChunkManager()->getWorldSize();
//     const auto worldSizeHalf = worldSize / 2;
//     int xMin = worldSize.x;
//     int xMax = -worldSize.x;
//     int yMin = worldSize.y;
//     int yMax = -worldSize.y;
//     int zMin = worldSize.z;
//     int zMax = -worldSize.z;

//     for (int x = -worldSizeHalf.x; x < worldSizeHalf.x; x++)
//     {
//         for (int z = -worldSizeHalf.z; z < worldSizeHalf.z; z++)
//         {
//             for (int y = -worldSizeHalf.y; y < worldSizeHalf.y; y++)
//             {
//                 int blockID = editorWorld->getChunkManager()->getBlock({x, y, z});
//                 if (blockID > 1 || (blockID != 0 && y != -worldSizeHalf.y))
//                 {
//                     xMin = std::min(xMin, x);
//                     yMin = std::min(yMin, y);
//                     zMin = std::min(zMin, z);
//                     xMax = std::max(xMax, x);
//                     yMax = std::max(yMax, y);
//                     zMax = std::max(zMax, z);
//                 }
//             }
//         }
//     }

//     buildData.buildSize = {
//         std::abs(xMax - xMin) + 1, std::abs(yMax - yMin) + 1, std::abs(zMax - zMin) + 1};

//     for (int x = xMin; x <= xMax; x++)
//     {
//         for (int y = yMin; y <= yMax; y++)
//         {
//             for (int z = zMin; z <= zMax; z++)
//             {
//                 buildData.blocks.emplace_back(
//                     editorWorld->getChunkManager()->getBlock({x, y, z}));
//             }
//         }
//     }

//     return buildData;
// }