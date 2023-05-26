#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entt.hpp>
#include "core/vs_app.h"
#include "core/vs_debug_draw.h"
#include "game/components/description.h"
#include "game/components/inputs.h"
#include "game/components/hoverable.h"
#include "game/components/bounds.h"
#include "game/components/ui_context.h"
#include "game/components/world_context.h"
#include "world/vs_world.h"

void updateSelectionSystem(entt::registry& mainRegistry)
{
    auto& inputs = mainRegistry.ctx().get<Inputs>();

    const auto& worldContext = mainRegistry.ctx().get<WorldContext>();

    auto& uiContext = mainRegistry.ctx().get<UIContext>();

    if (inputs.leftButtonState == InputState::JustUp && !uiContext.anyWindowHovered)
    {
        if (inputs.hoverEntity != entt::null)
        {
            // Update selected building

            // Reset construction selected building
            uiContext.selectedBuilding = {""};

            uiContext.selectedBuildingEntity = inputs.hoverEntity;
            const auto description = mainRegistry.try_get<Description>(inputs.hoverEntity);
            if (description != nullptr)
            {
                uiContext.entityDescription = description->description;
            }
        }
        else
        {
            uiContext.selectedBuildingEntity = entt::null;
            uiContext.entityDescription = "";
        }
    }

    if (uiContext.selectedBuildingEntity != entt::null)
    {
        // Draw debug
        const auto location = mainRegistry.get<Location>(uiContext.selectedBuildingEntity);
        const auto bounds = mainRegistry.get<Bounds>(uiContext.selectedBuildingEntity);

        worldContext.world->getDebugDraw()->drawBox(
            {location + bounds.min, location + bounds.max}, {255, 255, 255});
    }
}
