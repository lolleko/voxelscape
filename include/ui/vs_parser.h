#pragma once

#include <filesystem>
#include "world/vs_chunk_manager.h"

// These free namespace functions handle file writes and reads as well as interaction with the json library.
namespace VSParser
{
    // Writes the world data to a json file using the nlohmann json library. Returns true if successful.
    bool writeToFile(const VSChunkManager::VSWorldData& worldData, std::filesystem::path path);

    // Writes the build data to a json file using the nlohmann json library. Returns true if successful.
    bool writeBuildToFile(const VSChunkManager::VSBuildingData& buildData, std::filesystem::path path);

    [[nodiscard]] VSChunkManager::VSWorldData readFromFile(std::filesystem::path path);

    [[nodiscard]] VSChunkManager::VSBuildingData readBuildFromFile(std::filesystem::path path);
}
