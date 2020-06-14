#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <glm/fwd.hpp>
#include "core/vs_log.h"
#include "game/components/blocks.h"
#include "game/components/generator.h"
#include "game/components/hoverable.h"
#include "game/components/inputs.h"
#include "game/components/bounds.h"
#include "game/components/unique.h"
#include "game/components/world_context.h"

void updatePlacementSystem(entt::registry& mainRegistry, entt::registry& buildingTemplateRegistry)
{
    auto& inputs = mainRegistry.ctx<Inputs>();

    const auto& worldContext = mainRegistry.ctx<WorldContext>();

    const auto mouseLocation = glm::floor(inputs.mouseTrace.hitLocation);

    if (inputs.mouseTrace.bHasHit && inputs.leftButtonState == InputState::JustUp &&
        worldContext.bounds.isLocationInside(mouseLocation))
    {
        entt::entity selectedBuildingTemplate = entt::null;
        buildingTemplateRegistry.view<Unique>().each(
            [&selectedBuildingTemplate](const auto ent, const Unique& unique) {
                // TODO comparew with inputContext selected buildings
                if (unique.uuid == "building_lumberjack")
                {
                    selectedBuildingTemplate = ent;
                }
            });

        if (selectedBuildingTemplate != entt::null)
        {
            // TODO check and spend resource
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
                mainRegistry.assign<Location>(buildingInstance, newBuildingLocation);
                mainRegistry.assign<Bounds>(buildingInstance, selectedBuildingTemplateBounds);
                mainRegistry.assign<Blocks>(buildingInstance, templateBlocks);
                mainRegistry.assign<Generator>(buildingInstance, templateGenerator);
                mainRegistry.assign<Hoverable>(buildingInstance, Color(255, 0, 0));

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
