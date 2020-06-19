#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <glm/fwd.hpp>
#include "core/vs_log.h"
#include "core/vs_app.h"
#include "game/components/blocks.h"
#include "game/components/generator.h"
#include "game/components/hoverable.h"
#include "game/components/inputs.h"
#include "game/components/bounds.h"
#include "game/components/player.h"
#include "game/components/resourceamount.h"
#include "game/components/unique.h"
#include "game/components/world_context.h"

void updatePlacementSystem(entt::registry& mainRegistry, entt::registry& buildingTemplateRegistry)
{
    auto& inputs = mainRegistry.ctx<Inputs>();

    const auto& worldContext = mainRegistry.ctx<WorldContext>();

    const auto mouseLocation = glm::floor(inputs.mouseTrace.hitLocation);

    if (inputs.mouseTrace.bHasHit && inputs.leftButtonState == InputState::JustUp &&
        !inputs.anyWindowHovered && worldContext.bounds.isLocationInside(mouseLocation))
    {
        entt::entity selectedBuildingTemplate = entt::null;
        buildingTemplateRegistry.view<Unique>().each(
            [&selectedBuildingTemplate, &inputs](const auto ent, const Unique& unique) {
                // TODO comparew with inputContext selected buildings
                if (unique.uuid == inputs.selectedBuilding.uuid)
                {
                    selectedBuildingTemplate = ent;
                }
            });

        if (selectedBuildingTemplate != entt::null)
        {
            // TODO check and spend resource
            {
                bool resourcesSufficient = true;

                auto& player = mainRegistry.ctx<Player>();
                const auto& cost =
                    buildingTemplateRegistry.try_get<ResourceAmount>(selectedBuildingTemplate);

                for (auto& resourceAmount : player.resources.resourceVector)
                {
                    if (cost->resource.uuid == resourceAmount.resource.uuid)
                    {
                        if (cost->amount > resourceAmount.amount)
                        {
                            resourcesSufficient = false;
                        }
                        else
                        {
                            resourceAmount.amount -= cost->amount;
                        }
                    }
                }

                if (!resourcesSufficient)
                {
                    return;
                }
            }

            auto intersect = false;

            const auto& selectedBuildingTemplateBounds =
                buildingTemplateRegistry.get<Bounds>(selectedBuildingTemplate);

            const auto newBuildingLocation = glm::floor(mouseLocation) + glm::vec3(0.5F, 0.F, 0.5F);

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

            // TODO intersection test with blocks

            if (!intersect)
            {
                const auto [templateBlocks, templateGenerator] =
                    buildingTemplateRegistry.get<Blocks, Generator>(selectedBuildingTemplate);

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
            }
        }
    }
}
