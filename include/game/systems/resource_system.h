#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>

#include "core/vs_app.h"
#include "game/components/generator.h"
#include "game/components/unique.h"
#include "game/components/world_context.h"
#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"

#include <iostream>
void updateResourceSystem(entt::registry& registry)
{
    auto* UI = VSApp::getInstance()->getUI();

    const auto& worldContext = registry.ctx<WorldContext>();

    int woodCount = UI->getState()->woodCount;
    int stoneCount = UI->getState()->stoneCount;

    // Iterate over all instances, if generator is attached than increment resource
    registry.view<Generator>().each(
        [&worldContext, &woodCount, &stoneCount](Generator& generator) {
            // Only look for lumber and stone for now
            if (worldContext.worldAge - generator.lastGeneration > generator.interval)
            {
                if (generator.resource.uuid == "lumber")
                {
                    woodCount += generator.amount;
                }
                else if (generator.resource.uuid == "stone")
                {
                    stoneCount += generator.amount;
                }
                generator.lastGeneration = worldContext.worldAge;
            }
        });

    UI->getMutableState()->woodCount = woodCount;
    UI->getMutableState()->stoneCount = stoneCount;
}