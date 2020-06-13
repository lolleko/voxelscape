#pragma once

#include <entt/entt.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "game/resources.h"
#include "ui/vs_parser.h"
#include "world/vs_chunk_manager.h"

#include "game/components/generator.h"
#include "game/components/blocks.h"
#include "game/components/bounds.h"
#include "game/components/unique.h"

namespace BuildingParser
{
    void createBuildingFromFile(
        std::filesystem::path buildingDirectory,
        entt::registry& buildingRegistry)
    {
        const auto buildingEnt = buildingRegistry.create();

        const auto blocksFilePath = buildingDirectory / "blocks.json";
        if (!std::filesystem::exists(blocksFilePath))
        {
            // TODO handle
            assert(false);
            return;
        }

        const auto blocks = VSParser::readBuildFromFile(blocksFilePath);

        buildingRegistry.assign<Blocks>(buildingEnt, blocks.blocks, blocks.buildSize);
        buildingRegistry.assign<Bounds>(
            buildingEnt, -glm::vec3(blocks.buildSize) / 2.F, glm::vec3(blocks.buildSize) / 2.F);

        const auto componentsFilePath = buildingDirectory / "components.json";
        if (!std::filesystem::exists(componentsFilePath))
        {
            // TODO handle
            assert(false);
            return;
        }

        std::ifstream componentsFile;

        componentsFile.open(componentsFilePath);

        // TODO: Implement, checks, there is a lot of stuff that can go wrong
        nlohmann::json componentJson;
        componentsFile >> componentJson;

        if (!componentJson.contains("uuid"))
        {
            // TODO handle
            assert(false);
            return;
        }

        buildingRegistry.assign<Unique>(componentJson.at("uuid"));

        const auto generators = componentJson.find("generators");
        if (generators != componentJson.end() && generators->is_array())
        {
            for (auto& generator : generators.value())
            {
                // TODO translate strings to enums (get actual resource)
                Resources resource = Resources::Lumber;
                std::uint32_t amount = generator.at("amount");
                float interval = generator.at("interval");
                float lastGeneration = 0;

                buildingRegistry.assign<Generator>(
                    buildingEnt, resource, amount, interval, lastGeneration);
            }
        }
    };

}  // namespace BuildingParser
