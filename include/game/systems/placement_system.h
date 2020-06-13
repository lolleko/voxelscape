#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entt.hpp>
#include "game/buildings.h"
#include "game/components/inputs.h"
#include "game/components/bounds.h"

void updatePlacementSystem(entt::registry& mainRegistry, entt::registry& /*buildingRegistry*/)
{
    auto& inputs = mainRegistry.ctx<Inputs>();

    if (inputs.mouseTrace.bHasHit && inputs.leftButtonState == InputState::JustUp &&
        inputs.selectedBuilding != Buildings::None)
    {
        // TODO check and spend resource
        auto intersect = false;

        // Intersect with other entities
        mainRegistry.view<Location, Bounds>().each([&inputs, &intersect](
                                                       auto /*unused*/,
                                                       const Location& location,
                                                       const Bounds& bounds) {
            // TODO move bounds check to helper function
            const auto mouseLocationLocal = inputs.mouseTrace.hitLocation - location;
            if ((mouseLocationLocal.x >= bounds.min.x && mouseLocationLocal.x <= bounds.max.x) &&
                (mouseLocationLocal.y >= bounds.min.y && mouseLocationLocal.y <= bounds.max.y) &&
                (mouseLocationLocal.z >= bounds.min.z && mouseLocationLocal.z <= bounds.max.z))
            {
                intersect = true;
            }
        });

        // TODO intersection test with blocks

        if (!intersect)
        {
            switch (inputs.selectedBuilding)
            {
                case Buildings::None:
                    break;
                case Buildings::LumberJack:
                    // todo registry create<Generator, Bounds>...
                    break;
                case Buildings::StoneCutter:
                    break;
                case Buildings::Mine:
                    break;
            }
        }
    }
}
