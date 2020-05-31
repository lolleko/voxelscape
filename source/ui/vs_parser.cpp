#include "ui/vs_parser.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include "world/vs_block.h"
#include "world/vs_chunk_manager.h"

namespace VSParser
{
    bool writeToFile(VSChunkManager::VSWorldData& worldData, std::filesystem::path path)
    {
        std::ofstream outFile;
        outFile.open(path);
        // TODO: Check if file is empty etc.

        nlohmann::json json;
        json["chunkCount"] = {worldData.chunkCount.x, worldData.chunkCount.y};
        json["chunkSize"] = {worldData.chunkSize.x, worldData.chunkSize.y, worldData.chunkSize.z};
        json["blocks"] = worldData.blocks;
        outFile /*<< std::setw(4)*/ << json << std::endl;
        outFile.close();
        return true;
    }

    VSChunkManager::VSWorldData readFromFile(std::filesystem::path path)
    {
        VSChunkManager::VSWorldData worldData;
        if (!std::filesystem::exists(path))
        {
            return VSChunkManager::VSWorldData{};
        }
        std::ifstream inFile;

        inFile.open(path);

        // TODO: Implement, checks, there is a lot of stuff that can go wrong
        nlohmann::json json;
        inFile >> json;
        auto const blocksPos = json.find("blocks");
        auto const chunkCountPos = json.find("chunkCount");
        auto const chunkSizePos = json.find("chunkSize");
        if (blocksPos == json.end() || chunkCountPos == json.end() || chunkSizePos == json.end())
        {
            // Did not find all the necessary keys
            return VSChunkManager::VSWorldData{};
        }

        // Parse block data
        json.at("blocks").get_to(worldData.blocks);
        std::vector<int> chunkSizeVec(3);
        json.at("chunkSize").get_to(chunkSizeVec);
        std::vector<int> chunkCountVec(2);
        json.at("chunkCount").get_to(chunkCountVec);
        worldData.chunkCount.x = chunkCountVec.at(0);
        worldData.chunkCount.y = chunkCountVec.at(1);
        worldData.chunkSize.x = chunkSizeVec.at(0);
        worldData.chunkSize.y = chunkSizeVec.at(1);
        worldData.chunkSize.z = chunkSizeVec.at(2);

        return worldData;
    }
}  // namespace VSParser
