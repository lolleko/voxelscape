#include "game/systems/population_system.h"
#include "game/components/player.h"
#include "game/components/population.h"
#include <cstdint>
#include <entt/entity/entity.hpp>

bool checkTemplatePopulationSpace(
    entt::registry& mainRegistry,
    entt::registry& buildingTemplateRegistry,
    entt::entity building)
{
    auto& player = mainRegistry.ctx<Player>();

    if (building == entt::null)
    {
        return false;
    }

    Unique* buildingName = buildingTemplateRegistry.try_get<Unique>(building);

    if (buildingName == nullptr)
    {
        return false;
    }

    std::int32_t buildingPopulation =
        getBuildingPopulationFromTemplateRegistry(buildingTemplateRegistry, *buildingName);

    return (player.population.populationSpace + buildingPopulation >= 0);
}

void updatePlayerPopulationWithTemplate(
    entt::registry& mainRegistry,
    entt::registry& buildingTemplateRegistry,
    entt::entity building)
{
    auto& player = mainRegistry.ctx<Player>();

    Unique buildingName = buildingTemplateRegistry.get<Unique>(building);
    std::int32_t buildingPopulation =
        getBuildingPopulationFromTemplateRegistry(buildingTemplateRegistry, buildingName);

    player.population.populationSpace += buildingPopulation;
}

void unemployPopulationFromEntity(
    entt::registry& mainRegistry,
    entt::registry& buildingTemplateRegistry,
    entt::entity building)
{
    auto& player = mainRegistry.ctx<Player>();
    Unique buildingName = mainRegistry.get<Unique>(building);
    const auto& buildingPopulation =
        getBuildingPopulationFromTemplateRegistry(buildingTemplateRegistry, buildingName);

    player.population.populationSpace -= buildingPopulation;
}

std::int32_t getBuildingPopulationFromTemplateRegistry(
    entt::registry& buildingTemplateRegistry,
    Unique buildingName)
{
    std::int32_t population = 0;

    buildingTemplateRegistry.view<Unique, Population>().each(
        [&buildingName, &population](const Unique& unique, Population& popspace) {
            if (unique.uuid == buildingName.uuid)
            {
                population = popspace.populationSpace;
            }
        });

    return population;
}