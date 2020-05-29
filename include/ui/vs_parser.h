#pragma once

#include <filesystem>
#include "world/vs_chunk_manager.h"

// These free namespace functions handle file writes and reads as well as interaction with the json library.
namespace VSParser
{
    // Writes the world data to a json file using the nlohmann json library. Returns true if successful.
    bool writeToFile(VSChunkManager::VSWorldData& worldData, std::filesystem::path path);

    VSChunkManager::VSWorldData readFromFile(std::filesystem::path path);
}
