#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include "game/components/blocks.h"
#include "game/components/level.h"
#include "game/components/location.h"
#include "game/components/player.h"
#include "game/components/ui_context.h"
#include "game/components/unique.h"
#include "game/components/upgradecost.h"
#include "game/components/world_context.h"

bool checkResourcesUpgrade(entt::registry& mainRegistry, Upgradecost cost)
{
    auto& player = mainRegistry.ctx<Player>();

    for (auto& resourceAmount : player.resources.resourceVector)
    {
        if (cost.resource.uuid == resourceAmount.resource.uuid)
        {
            if (cost.amount > resourceAmount.amount)
            {
                return false;
            }
        }
    }

    return true;
}

void spendResourcesUpgrade(entt::registry& mainRegistry, Upgradecost cost)
{
    auto& player = mainRegistry.ctx<Player>();

    for (auto& resourceAmount : player.resources.resourceVector)
    {
        if (cost.resource.uuid == resourceAmount.resource.uuid)
        {
            resourceAmount.amount -= cost.amount;
        }
    }
}

void upgradeBuilding(entt::registry& mainRegistry, entt::registry& buildingTemplateRegistry)
{
    const auto& worldContext = mainRegistry.ctx<WorldContext>();

    auto& uiContext = mainRegistry.ctx<UIContext>();

    if (uiContext.selectedBuildingEntity != entt::null)
    {
        const auto buildingType = mainRegistry.get<Unique>(uiContext.selectedBuildingEntity);

        entt::entity buildingTemplate = entt::null;

        // Match to existing buildings and get upgrade cost
        buildingTemplateRegistry.view<Unique>().each(
            [&buildingTemplate, &buildingType](const entt::entity entity, const Unique& unique) {
                if (unique.uuid == buildingType.uuid)
                {
                    buildingTemplate = entity;
                }
            });

        if (buildingTemplate == entt::null)
        {
            return;
        }

        const auto maxLevel = buildingTemplateRegistry.get<Level>(buildingTemplate);
        const auto templateBlocks = buildingTemplateRegistry.get<Blocks>(buildingTemplate);

        // Check if still another level to upgrade
        Level& currentLevel = mainRegistry.get<Level>(uiContext.selectedBuildingEntity);
        if (currentLevel.level < maxLevel.level)
        {
            const auto upgradeCost = buildingTemplateRegistry.get<Upgradecost>(buildingTemplate);

            // Still another level to upgrade
            if (checkResourcesUpgrade(mainRegistry, upgradeCost))
            {
                spendResourcesUpgrade(mainRegistry, upgradeCost);

                // Place blocks and increase level
                currentLevel.level += 1;

                const auto bounds = mainRegistry.get<Bounds>(uiContext.selectedBuildingEntity);
                const auto location = mainRegistry.get<Location>(uiContext.selectedBuildingEntity);

                const auto low = location + bounds.min;
                const auto high = location + bounds.max;

                for (int x = low.x; x < high.x; x++)
                {
                    for (int y = low.y; y < high.y; y++)
                    {
                        for (int z = low.z; z < high.z; z++)
                        {
                            worldContext.world->getChunkManager()->setBlock(
                                {x, y, z},
                                templateBlocks.blocks.at(currentLevel.level - 1)
                                    [(x - low.x) + (y - low.y) * templateBlocks.size.x +
                                     (z - low.z) * templateBlocks.size.x * templateBlocks.size.y]);
                        }
                    }
                }
            }
        }
    }
}