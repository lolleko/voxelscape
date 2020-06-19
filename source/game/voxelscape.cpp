#include "game/voxelscape.h"

#include "game/components/resourceamount.h"
#include "game/components/resources.h"
#include "game/components/unique.h"
#include "game/components/world_context.h"
#include "game/components/player.h"

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
    const auto UI = getApp()->getUI();

    // TODO the app/mainloop should be never allowed to change the active world
    // after initailization otherwhise this could crash
    if (getApp()->getWorldName() == VSGame::WorldName)
    {
        const auto* prevWorldContext = mainRegistry.try_ctx<WorldContext>();
        if (prevWorldContext == nullptr)
        {
            // Init player

            // Init Resources
            auto resources = Resources{};
            unsigned int woodCount = UI->getState()->woodCount;
            unsigned int stoneCount = UI->getState()->stoneCount;

            const auto wood = ResourceAmount{Unique{"lumber"}, woodCount};
            const auto stone = ResourceAmount{Unique{"stone"}, stoneCount};

            resources.resourceVector.emplace_back(wood);
            resources.resourceVector.emplace_back(stone);

            mainRegistry.set<Player>(resources);

            // Init world context
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

            // Update player resources in UI
            const auto& player = mainRegistry.ctx<Player>();

            for (const auto& resourceAmount : player.resources.resourceVector)
            {
                if (resourceAmount.resource.uuid == "lumber")
                {
                    UI->getMutableState()->woodCount = resourceAmount.amount;
                }
                if (resourceAmount.resource.uuid == "stone")
                {
                    UI->getMutableState()->stoneCount = resourceAmount.amount;
                }
            }
        }
    }
}