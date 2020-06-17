#include "game/voxelscape.h"

#include "game/components/world_context.h"

#include "game/systems/input_system.h"
#include "game/systems/hover_system.h"
#include "game/systems/placement_system.h"
#include "game/systems/resource_system.h"

#include "game/building_loader.h"

void Voxelscape::initializeGame(VSApp* inApp)
{
    (void)inApp;
    BuildingParser::createBuildingFromFile("resources/buildings/lumberjack", buildingRegistry);
    BuildingParser::createBuildingFromFile("resources/buildings/stonemine", buildingRegistry);
}

void Voxelscape::update(float deltaSeconds)
{
    // TODO the app/mainloop should be never allowed to change the active world
    // after initailization otherwhise this could crash
    if (getApp()->getWorldName() == VSGame::WorldName)
    {
        const auto* prevWorldContext = mainRegistry.try_ctx<WorldContext>();
        if (prevWorldContext == nullptr)
        {
            mainRegistry.set<WorldContext>(
                getApp()->getWorld(),
                deltaSeconds,
                /*worldAge*/ 0.F,
                Bounds{-getApp()->getWorld()->getChunkManager()->getWorldSize() / 2,
                       getApp()->getWorld()->getChunkManager()->getWorldSize() / 2});
            updateInputSystem(mainRegistry);
            updateHoverSystem(mainRegistry);
            updatePlacementSystem(mainRegistry, buildingRegistry);
            updateResourceSystem(mainRegistry);
        }
        else
        {
            mainRegistry.set<WorldContext>(
                getApp()->getWorld(),
                deltaSeconds,
                prevWorldContext->worldAge + deltaSeconds,
                Bounds{-getApp()->getWorld()->getChunkManager()->getWorldSize() / 2,
                       getApp()->getWorld()->getChunkManager()->getWorldSize() / 2});
            updateInputSystem(mainRegistry);
            updateHoverSystem(mainRegistry);
            updatePlacementSystem(mainRegistry, buildingRegistry);
            updateResourceSystem(mainRegistry);
        }
    }
}