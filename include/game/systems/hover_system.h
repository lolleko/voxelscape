#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entt.hpp>
#include "core/vs_app.h"
#include "core/vs_debug_draw.h"
#include "game/components/inputs.h"
#include "game/components/hoverable.h"
#include "game/components/bounds.h"
#include "world/vs_world.h"

void updateHoverSystem(entt::registry& registry)
{
    auto& inputs = registry.ctx<Inputs>();

    inputs.hoverEntity = entt::null;

    registry.view<Hoverable, Location, Bounds>().each([&inputs](
                                                          auto entity,
                                                          const Hoverable& hoverable,
                                                          const Location& location,
                                                          const Bounds& bounds) {
        // TODO move bounds check to helper function
        const auto mouseLocationLocal = inputs.worldMouse - location;
        if ((mouseLocationLocal.x >= bounds.min.x && mouseLocationLocal.x <= bounds.max.x) &&
            (mouseLocationLocal.y >= bounds.min.y && mouseLocationLocal.y <= bounds.max.y) &&
            (mouseLocationLocal.z >= bounds.min.z && mouseLocationLocal.z <= bounds.max.z))
        {
            inputs.hoverEntity = entity;
            // Add visual indicator
            VSApp::getInstance()->getActiveWorld()->getDebugDraw()->drawBox(
                {location + bounds.min, location + bounds.max}, hoverable.hoverColor);
            // For now stop on the first intersection
            return;
        }
    });
}
