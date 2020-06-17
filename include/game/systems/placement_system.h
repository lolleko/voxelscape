#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <glm/fwd.hpp>
#include "core/vs_log.h"
#include "core/vs_app.h"
#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"
#include "game/components/blocks.h"
#include "game/components/generator.h"
#include "game/components/hoverable.h"
#include "game/components/inputs.h"
#include "game/components/bounds.h"
#include "game/components/unique.h"
#include "game/components/world_context.h"
#include "game/components/cost.h"

void updatePlacementSystem(entt::registry& mainRegistry, entt::registry& buildingTemplateRegistry)
{
    auto& inputs = mainRegistry.ctx<Inputs>();

    const auto& worldContext = mainRegistry.ctx<WorldContext>();

    auto* UI = VSApp::getInstance()->getUI();

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

                unsigned int woodCount = UI->getState()->woodCount;
                unsigned int stoneCount = UI->getState()->stoneCount;

                const auto& cost = buildingTemplateRegistry.try_get<Cost>(selectedBuildingTemplate);

                if (cost != nullptr)
                {
                    if (cost->resource.uuid == "lumber")
                    {
                        if (cost->amount > woodCount)
                        {
                            resourcesSufficient = false;
                        }
                        else
                        {
                            UI->getMutableState()->woodCount = woodCount - cost->amount;
                        }
                    }
                    else if (cost->resource.uuid == "stone")
                    {
                        if (cost->amount > stoneCount)
                        {
                            resourcesSufficient = false;
                        }
                        else
                        {
                            UI->getMutableState()->stoneCount = stoneCount - cost->amount;
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
