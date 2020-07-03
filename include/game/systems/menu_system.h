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
        world->getChunkManager()->setChunkDimensions(uiContext.chunkSize, uiContext.chunkCount);
        uiContext.bShouldUpdateChunks = false;
    }

    if (uiContext.bShouldGenerateTerrain && !world->getChunkManager()->shouldReinitializeChunks())
    {
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
        uiContext.bShouldGenerateTerrain = false;

        if (!uiContext.bEditorActive)
        {
            uiContext.minimap.bShouldUpdate = true;
        }
    }
}