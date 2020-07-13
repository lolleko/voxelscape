#include "game/systems/upgrade_system.h"
#include "game/components/blocks.h"
#include "game/components/description.h"
#include "game/components/location.h"
#include "game/components/player.h"
#include "game/components/resourceamount.h"
#include "game/components/ui_context.h"
#include "game/components/unique.h"
#include "game/components/rotated.h"
#include "game/components/world_context.h"
#include "game/components/upgrade.h"
#include "game/systems/population_system.h"
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
        const auto upgradeDescription =
            buildingTemplateRegistry.try_get<Description>(upgradeTemplate);

        // Still another level to upgrade
        // unemployPopulationFromEntity(
        //     mainRegistry, buildingTemplateRegistry, uiContext.selectedBuildingEntity);
        if (checkResources(mainRegistry, buildingTemplateRegistry, upgradeTemplate) /*&&
            checkTemplatePopulationSpace(mainRegistry, buildingTemplateRegistry, upgradeTemplate)*/)
        {
            spendResources(mainRegistry, buildingTemplateRegistry, upgradeTemplate);
            // updatePlayerPopulationWithTemplate(
            //     mainRegistry, buildingTemplateRegistry, upgradeTemplate);

            const auto bounds = buildingTemplateRegistry.get<Bounds>(upgradeTemplate);
            const auto rotated = mainRegistry.get<Rotated>(uiContext.selectedBuildingEntity);
            const auto location = mainRegistry.get<Location>(uiContext.selectedBuildingEntity);

            placeBuildingBlocks(worldContext, bounds, location, upgradeBlocks, rotated.bIsRotated);

            const auto buildingInstance = mainRegistry.create();
            mainRegistry.emplace<Unique>(buildingInstance, upgradeName);
            mainRegistry.emplace<Location>(buildingInstance, location);
            mainRegistry.emplace<Bounds>(buildingInstance, bounds);
            mainRegistry.emplace<Blocks>(buildingInstance, templateBlocks);
            mainRegistry.emplace<Rotated>(buildingInstance, rotated);
            if (upgradeGenerator != nullptr)
            {
                mainRegistry.emplace<Generator>(buildingInstance, *upgradeGenerator);
            }
            if (upgradeUpgrade != nullptr)
            {
                mainRegistry.emplace<Upgrade>(buildingInstance, *upgradeUpgrade);
            }
            if (upgradeDescription != nullptr)
            {
                mainRegistry.emplace<Description>(buildingInstance, *upgradeDescription);
            }
            mainRegistry.emplace<Hoverable>(buildingInstance, Color(255, 0, 0));

            // unemployPopulationFromEntity(
            //     mainRegistry, buildingTemplateRegistry, uiContext.selectedBuildingEntity);
            // mainRegistry.destroy(uiContext.selectedBuildingEntity);
            uiContext.selectedBuildingEntity = entt::null;
            uiContext.entityDescription = "";
        }
    }
}