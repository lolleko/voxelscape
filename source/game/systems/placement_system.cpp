#include "game/systems/placement_system.h"
#include <entt/entity/entity.hpp>
#include <glm/fwd.hpp>
#include <iostream>
#include <ostream>
#include "core/vs_input_handler.h"
#include "game/components/description.h"
#include "game/components/ui_context.h"
#include "game/components/upgrade.h"

void updatePlacementSystem(entt::registry& mainRegistry, entt::registry& buildingTemplateRegistry)
{
    const auto& inputs = mainRegistry.ctx<Inputs>();

    const auto& worldContext = mainRegistry.ctx<WorldContext>();

    auto& uiContext = mainRegistry.ctx<UIContext>();

    const auto mouseLocation = glm::floor(inputs.mouseTrace.hitLocation);

    if (inputs.mouseTrace.bHasHit && !uiContext.anyWindowHovered &&
        worldContext.bounds.isLocationInside(mouseLocation))
    {
        entt::entity selectedBuildingTemplate = entt::null;
        buildingTemplateRegistry.view<Unique>().each(
            [&selectedBuildingTemplate, &uiContext](const auto ent, const Unique& unique) {
                if (unique.uuid == uiContext.selectedBuilding.uuid)
                {
                    selectedBuildingTemplate = ent;
                }
            });

        if (selectedBuildingTemplate != entt::null)
        {
            auto intersect = false;

            const auto& selectedBuildingTemplateBounds =
                buildingTemplateRegistry.get<Bounds>(selectedBuildingTemplate);

            const auto& selectedBuildingTemplateName =
                buildingTemplateRegistry.get<Unique>(selectedBuildingTemplate);

            const auto newBuildingLocation = glm::floor(mouseLocation) + glm::vec3(0.5F, 0.F, 0.5F);

            // TODO: Make sure correct building level is selected, always first?
            const auto templateBlocks =
                buildingTemplateRegistry.get<Blocks>(selectedBuildingTemplate);

            const auto templateGenerator =
                buildingTemplateRegistry.try_get<Generator>(selectedBuildingTemplate);

            const auto templateUpgrade = buildingTemplateRegistry.try_get<Upgrade>(selectedBuildingTemplate);

            const auto templateDescription = buildingTemplateRegistry.try_get<Description>(selectedBuildingTemplate);

            auto* previewChunkManager = worldContext.world->getPreviewChunkManager();

            if (!uiContext.bIsBuildingPreviewInitialized)
            {
                previewChunkManager->setChunkDimensions(templateBlocks.size * 3, {2, 2});
                uiContext.bIsBuildingPreviewInitialized = true;
            }

            if (uiContext.bIsBuildingPreviewInitialized &&
                !previewChunkManager->shouldReinitializeChunks() &&
                !uiContext.bIsBuildingPreviewConstructed)
            {
                for (int x = 0; x < templateBlocks.size.x; x++)
                {
                    for (int y = 0; y < templateBlocks.size.y; y++)
                    {
                        for (int z = 0; z < templateBlocks.size.z; z++)
                        {
                            const auto offset = glm::vec3(
                                selectedBuildingTemplateBounds.min.x,
                                0,
                                selectedBuildingTemplateBounds.min.z);
                            previewChunkManager->setBlock(
                                glm::vec3{x, y, z} + offset,
                                templateBlocks.blocks
                                    [x + y * templateBlocks.size.x +
                                     z * templateBlocks.size.x * templateBlocks.size.y]);
                        }
                    }
                }
                uiContext.bIsBuildingPreviewConstructed = true;
            }

            if (uiContext.bIsBuildingPreviewConstructed)
            {
                previewChunkManager->setOrigin(glm::ceil(mouseLocation) + glm::vec3{1, 0, 1});
            }

            // Intersect with other entities
            mainRegistry.view<Location, Bounds>().each(
                [&newBuildingLocation, &intersect, &selectedBuildingTemplateBounds](
                    const Location& location, const Bounds& bounds) {
                    const auto newBuidlingWorldSpaceBounds =
                        (selectedBuildingTemplateBounds + newBuildingLocation);
                    const auto sampleBuildingWorldSpaceBounds = bounds + location;

                    if (sampleBuildingWorldSpaceBounds.isIntersecting(newBuidlingWorldSpaceBounds))
                    {
                        intersect = true;
                    }
                });

            if (intersect)
            {
                previewChunkManager->setColorOverride(glm::vec3{1.F, 0.3F, 0.3F});
            }
            else
            {
                previewChunkManager->setColorOverride(glm::vec3{0.3F, 1.F, 0.3F});
            }

            // TODO intersection test with blocks

            if (checkResources(mainRegistry, buildingTemplateRegistry, selectedBuildingTemplate) &&
                !intersect && inputs.leftButtonState == InputState::JustUp)
            {
                spendResources(mainRegistry, buildingTemplateRegistry, selectedBuildingTemplate);

                const auto buildingInstance = mainRegistry.create();
                mainRegistry.emplace<Unique>(buildingInstance, selectedBuildingTemplateName);
                mainRegistry.emplace<Location>(buildingInstance, newBuildingLocation);
                mainRegistry.emplace<Bounds>(buildingInstance, selectedBuildingTemplateBounds);
                mainRegistry.emplace<Blocks>(buildingInstance, templateBlocks);
                if (templateGenerator != nullptr)
                {
                    mainRegistry.emplace<Generator>(buildingInstance, *templateGenerator);
                }
                if (templateUpgrade != nullptr)
                {
                    mainRegistry.emplace<Upgrade>(buildingInstance, *templateUpgrade);
                }
                if (templateDescription != nullptr)
                {
                    mainRegistry.emplace<Description>(buildingInstance, *templateDescription);
                }
                mainRegistry.emplace<Hoverable>(buildingInstance, Color(255, 0, 0));

                for (int x = 0; x < templateBlocks.size.x; x++)
                {
                    for (int y = 0; y < templateBlocks.size.y; y++)
                    {
                        for (int z = 0; z < templateBlocks.size.z; z++)
                        {
                            worldContext.world->getChunkManager()->setBlock(
                                newBuildingLocation + glm::vec3{x, y, z} +
                                    selectedBuildingTemplateBounds.min,
                                templateBlocks.blocks
                                    [x + y * templateBlocks.size.x +
                                     z * templateBlocks.size.x * templateBlocks.size.y]);
                        }
                    }
                }

                if ((inputs.Up & VSInputHandler::KEY_SHIFT) != 0)
                {
                    uiContext.selectedBuilding = {""};
                }

                uiContext.minimap.bShouldUpdate = true;
            }
        }
        else
        {
            worldContext.world->getPreviewChunkManager()->setChunkDimensions({}, {});
            uiContext.bIsBuildingPreviewInitialized = false;
            uiContext.bIsBuildingPreviewConstructed = false;
        }
    }
}

bool checkResources(
    entt::registry& mainRegistry,
    entt::registry& buildingTemplateRegistry,
    entt::entity selectedBuildingTemplate)
{
    auto& player = mainRegistry.ctx<Player>();
    const auto& cost = buildingTemplateRegistry.try_get<ResourceAmount>(selectedBuildingTemplate);

    for (auto& resourceAmount : player.resources.resourceVector)
    {
        if (cost->resource.uuid == resourceAmount.resource.uuid)
        {
            if (cost->amount > resourceAmount.amount)
            {
                return false;
            }
        }
    }
    return true;
}

void spendResources(
    entt::registry& mainRegistry,
    entt::registry& buildingTemplateRegistry,
    entt::entity selectedBuildingTemplate)
{
    auto& player = mainRegistry.ctx<Player>();
    const auto& cost = buildingTemplateRegistry.try_get<ResourceAmount>(selectedBuildingTemplate);

    for (auto& resourceAmount : player.resources.resourceVector)
    {
        if (cost->resource.uuid == resourceAmount.resource.uuid)
        {
            resourceAmount.amount -= cost->amount;
        }
    }
}