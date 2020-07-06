#pragma once

#include <entt/entt.hpp>
#include "game/components/inputs.h"
#include "game/components/ui_context.h"
#include "game/components/world_context.h"
#include "world/generator/vs_terrain.h"
#include "core/vs_app.h"

void updateMenuSystem(entt::registry& mainRegistry)
{
    auto& uiContext = mainRegistry.ctx<UIContext>();

    auto* app = VSApp::getInstance();

    if (uiContext.bShouldSetEditorActive)
    {
        app->setWorldActive(uiContext.editorWorldName);
        uiContext.bShouldSetEditorActive = false;
    }
    if (uiContext.bShouldSetGameActive)
    {
        app->setWorldActive(uiContext.gameWorldName);
        uiContext.bShouldSetGameActive = false;
    }
    auto* world = app->getWorld();

    // Update world state with ui state
    if (uiContext.bShouldUpdateChunks)
    {
        glm::ivec2 chunkCount = {4, 4};

        if (uiContext.worldSize == 0)
        {
            // Small
            chunkCount = {16, 16};
        }
        else if (uiContext.worldSize == 1)
        {
            // Medium
            chunkCount = {32, 32};
        }
        else if (uiContext.worldSize == 2)
        {
            // Large
            chunkCount = {64, 64};
        }
        else if (uiContext.worldSize == 3)
        {
            // Tiny for debug
            chunkCount = {2, 2};
        }

        constexpr glm::ivec3 chunkSize = {16, 128, 16};

        world->getChunkManager()->setChunkDimensions(chunkSize, chunkCount);
        uiContext.bShouldUpdateChunks = false;
    }

    if (uiContext.bShouldGenerateTerrain && !world->getChunkManager()->shouldReinitializeChunks())
    {
        uiContext.bShouldGenerateTerrain = false;

        if (uiContext.selectedBiomeType == 0)
        {
            VSTerrainGeneration::buildStandard(world);
        }
        else if (uiContext.selectedBiomeType == 1)
        {
            VSTerrainGeneration::buildMountains(world);
        }
        else if (uiContext.selectedBiomeType == 2)
        {
            VSTerrainGeneration::buildDesert(world);
        }

        if (!uiContext.bEditorActive)
        {
            uiContext.minimap.bShouldUpdate = true;
        }
    }
}