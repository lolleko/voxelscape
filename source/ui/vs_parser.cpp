#include "ui/vs_parser.h"

#include <filesystem>
#include <nlohmann/json.hpp>
#include <iostream>
#include "world/vs_block.h"
#include "world/vs_chunk_manager.h"

namespace VSParser
{
    bool writeToFile(VSChunkManager::WorldData& worldData, std::filesystem::path path)
    {
        (void)worldData;
        (void)path;
        nlohmann::json json;
        json["chunkSize"] = {worldData.chunkSize.x, worldData.chunkSize.y, worldData.chunkSize.z};
        json["chunkCount"] = {worldData.chunkCount.x, worldData.chunkCount.y};
        json["blocks"] = worldData.blocks;
        std::cout << std::setw(4) << json << std::endl;
        return true;
    }

    VSChunkManager::WorldData readFromFile(std::filesystem::path path)
    {
        (void)path;
        VSChunkManager::WorldData worldData;

        // TODO: Implement, checks, there is a lot of stuff that can go wrong

        return worldData;
    }
}  // namespace VSParser