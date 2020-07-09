#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>

#include "core/vs_app.h"
#include "game/components/generator.h"
#include "game/components/player.h"
#include "game/components/unique.h"
#include "game/components/world_context.h"

#include <iostream>
void updateResourceSystem(entt::registry& registry)
{
    const auto& worldContext = registry.ctx<WorldContext>();

    auto& player = registry.ctx<Player>();
    // Iterate over all instances, if generator is attached than increment resource
    registry.view<Generator>().each([&worldContext, &player](Generator& generator) {
        // Only look for lumber and stone for now
        if (worldContext.worldAge - generator.lastGeneration > generator.interval)
        {
            for (auto& resourceAmount : player.resources.resourceVector)
            {
                if (generator.resource.uuid == resourceAmount.resource.uuid)
                {
                    resourceAmount.amount += generator.amount;
                }
            }
            generator.lastGeneration = worldContext.worldAge;
        }
    });
}