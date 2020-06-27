#include "game/systems/placement_system.h"
#include <glm/fwd.hpp>
#include <iostream>
#include <ostream>
#include "core/vs_input_handler.h"

void updatePlacementSystem(entt::registry& mainRegistry, entt::registry& buildingTemplateRegistry)
{
    auto& inputs = mainRegistry.ctx<Inputs>();

    const auto& worldContext = mainRegistry.ctx<WorldContext>();

    const auto mouseLocation = glm::floor(inputs.mouseTrace.hitLocation);

    if (inputs.mouseTrace.bHasHit && !inputs.anyWindowHovered &&
        worldContext.bounds.isLocationInside(mouseLocation))
    {
        entt::entity selectedBuildingTemplate = entt::null;
        buildingTemplateRegistry.view<Unique>().each(
            [&selectedBuildingTemplate, &inputs](const auto ent, const Unique& unique) {
                if (unique.uuid == inputs.selectedBuilding.uuid)
                {
                    selectedBuildingTemplate = ent;
                }
            });

        if (selectedBuildingTemplate != entt::null)
        {
            auto intersect = false;

            const auto& selectedBuildingTemplateBounds =
                buildingTemplateRegistry.get<Bounds>(selectedBuildingTemplate);

            const auto newBuildingLocation = glm::floor(mouseLocation) + glm::vec3(0.5F, 0.F, 0.5F);

            const auto [templateBlocks, templateGenerator] =
                buildingTemplateRegistry.get<Blocks, Generator>(selectedBuildingTemplate);

            auto* previewChunkManager = worldContext.world->getPreviewChunkManager();

            if (!inputs.bIsBuildingPreviewInitialized)
            {
                previewChunkManager->setChunkDimensions(templateBlocks.size * 3, {2, 2});
                inputs.bIsBuildingPreviewInitialized = true;
            }

            if (inputs.bIsBuildingPreviewInitialized &&
                !previewChunkManager->shouldReinitializeChunks() &&
                !inputs.bIsBuildingPreviewConstructed)
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
                inputs.bIsBuildingPreviewConstructed = true;
            }

            if (inputs.bIsBuildingPreviewConstructed)
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
                previewChunkManager->setColorOverride(glm::vec3{1.F, 0.5F, 0.5F});
            }
            else
            {
                previewChunkManager->setColorOverride(glm::vec3{0.5F, 1.F, 0.5F});
            }

            // TODO intersection test with blocks

            if (checkResources(mainRegistry, buildingTemplateRegistry, selectedBuildingTemplate) &&
                !intersect && inputs.leftButtonState == InputState::JustUp)
            {
                spendResources(mainRegistry, buildingTemplateRegistry, selectedBuildingTemplate);

                const auto buildingInstance = mainRegistry.create();
                mainRegistry.emplace<Location>(buildingInstance, newBuildingLocation);
                mainRegistry.emplace<Bounds>(buildingInstance, selectedBuildingTemplateBounds);
                mainRegistry.emplace<Blocks>(buildingInstance, templateBlocks);
                mainRegistry.emplace<Generator>(buildingInstance, templateGenerator);
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
                    inputs.bShouldResetSelection = true;
                }
            }
        }
        else
        {
            worldContext.world->getPreviewChunkManager()->setChunkDimensions({}, {});
            inputs.bIsBuildingPreviewInitialized = false;
            inputs.bIsBuildingPreviewConstructed = false;
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