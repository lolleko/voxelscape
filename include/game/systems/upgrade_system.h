#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include "game/components/blocks.h"
#include "game/components/location.h"
#include "game/components/player.h"
#include "game/components/resourceamount.h"
#include "game/components/ui_context.h"
#include "game/components/unique.h"
#include "game/components/world_context.h"
#include "game/components/upgrade.h"
#include "game/systems/placement_system.h"


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

        const auto upgradePtr = buildingTemplateRegistry.try_get<Upgrade>(buildingTemplate);

        if (upgradePtr == nullptr)
        {
            // No upgrade available
            return;
        }

        const auto upgrade = buildingTemplateRegistry.get<Upgrade>(buildingTemplate);

        entt::entity upgradeTemplate = entt::null;

        // Match upgrade to existing buildings and get upgrade cost
        buildingTemplateRegistry.view<Unique>().each(
            [&upgradeTemplate, &upgrade](const entt::entity entity, const Unique& unique) {
                if (unique.uuid == upgrade.name.uuid)
                {
                    upgradeTemplate = entity;
                }
            });

        const auto templateBlocks = buildingTemplateRegistry.get<Blocks>(upgradeTemplate);
        const auto upgradeName = buildingTemplateRegistry.get<Unique>(upgradeTemplate);
        const auto upgradeCost = buildingTemplateRegistry.get<ResourceAmount>(upgradeTemplate);
        const auto upgradeBlocks = buildingTemplateRegistry.get<Blocks>(upgradeTemplate);
        const auto upgradeGenerator = buildingTemplateRegistry.try_get<Generator>(upgradeTemplate);
        const auto upgradeUpgrade = buildingTemplateRegistry.try_get<Upgrade>(upgradeTemplate);

        // Still another level to upgrade
        if (checkResources(mainRegistry, buildingTemplateRegistry, upgradeTemplate))
        {
            spendResources(mainRegistry, buildingTemplateRegistry, upgradeTemplate);

            // assumes same bounds for now
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
                            upgradeBlocks.blocks
                                [(x - low.x) + (y - low.y) * upgradeBlocks.size.x +
                                 (z - low.z) * upgradeBlocks.size.x * upgradeBlocks.size.y]);
                    }
                }
            }

            const auto buildingInstance = mainRegistry.create();
            mainRegistry.emplace<Unique>(buildingInstance, upgradeName);
            mainRegistry.emplace<Location>(buildingInstance, location);
            mainRegistry.emplace<Bounds>(buildingInstance, bounds);
            mainRegistry.emplace<Blocks>(buildingInstance, templateBlocks);
            if (upgradeGenerator != nullptr)
            {
                mainRegistry.emplace<Generator>(buildingInstance, *upgradeGenerator);
            }
            if (upgradeUpgrade != nullptr)
            {
                mainRegistry.emplace<Upgrade>(buildingInstance, *upgradeUpgrade);
            }
            mainRegistry.emplace<Hoverable>(buildingInstance, Color(255, 0, 0));

            mainRegistry.destroy(uiContext.selectedBuildingEntity);
            uiContext.selectedBuildingEntity = entt::null;
        }
    }
}