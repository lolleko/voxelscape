#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>

#include <iostream>
#include <ostream>
#include "game/components/bounds.h"
#include "game/components/location.h"
#include "game/components/ui_context.h"
#include "game/components/world_context.h"

void deleteSelectedBuilding(entt::registry& mainRegistry)
{
    auto& uiContext = mainRegistry.ctx<UIContext>();
    auto& worldContext = mainRegistry.ctx<WorldContext>();

    const auto bounds = mainRegistry.get<Bounds>(uiContext.selectedBuildingEntity);
    const auto location = mainRegistry.get<Location>(uiContext.selectedBuildingEntity);

    const auto low = location + bounds.min;
    const auto high = location + bounds.max;

    for (int x = low.x; x < high.x; x++)
    {
        for (int y = low.y; y < high.y; y++)
        {
            for (int z = low.z; z < high.z; z++)
            {
                worldContext.world->getChunkManager()->setBlock({x, y, z}, 0);
            }
        }
    }

    mainRegistry.destroy(uiContext.selectedBuildingEntity);
}