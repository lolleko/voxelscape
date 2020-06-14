#pragma once

#include <entt/entt.hpp>
#include <filesystem>
#include <glm/fwd.hpp>
#include <nlohmann/json.hpp>

#include "core/vs_log.h"
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
        const glm::vec2 boundsXZ = {
            (glm::vec3(blocks.buildSize) / 2.F).x, (glm::vec3(blocks.buildSize) / 2.F).z};
        buildingRegistry.assign<Bounds>(
            buildingEnt,
            -glm::vec3(boundsXZ.x, 0.F, boundsXZ.y),
            glm::vec3(boundsXZ.x, blocks.buildSize.y, boundsXZ.y));

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

        VSLog::Log(VSLog::Category::Core, VSLog::Level::info, "{0}", componentJson.dump(4));

        if (!componentJson.contains("uuid"))
        {
            // TODO handle
            assert(false);
            return;
        }

        buildingRegistry.assign<Unique>(buildingEnt, componentJson.at("uuid"));

        if (componentJson.contains("generator"))
        {
            const auto generatorJSON = componentJson.at("generator");

            buildingRegistry.assign<Generator>(
                buildingEnt,
                generatorJSON.at("resource"),
                generatorJSON.at("amount"),
                generatorJSON.at("interval"),
                0);
        }
    };

}  // namespace BuildingParser
