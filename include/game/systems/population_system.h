#pragma once

#include <cstdint>
#include <entt/entity/entity.hpp>
#include <entt/entt.hpp>
#include "game/components/unique.h"

bool checkTemplatePopulationSpace(entt::registry& mainRegistry, entt::registry& buildingTemplateRegistry, entt::entity building);

void updatePlayerPopulationWithTemplate(entt::registry& mainRegistry, entt::registry& buildingTemplateRegistry, entt::entity building);

void unemployPopulationFromEntity(entt::registry& mainRegistry, entt::registry& buildingTemplateRegistry, entt::entity building);

std::int32_t getBuildingPopulationFromTemplateRegistry(entt::registry& buildingTemplateRegistry, Unique buildingName);