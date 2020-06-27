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

void updatePlacementSystem(entt::registry& mainRegistry, entt::registry& buildingTemplateRegistry);

bool checkResources(
    entt::registry& mainRegistry,
    entt::registry& buildingTemplateRegistry,
    entt::entity selectedBuildingTemplate);

void spendResources(
    entt::registry& mainRegistry,
    entt::registry& buildingTemplateRegistry,
    entt::entity selectedBuildingTemplate);