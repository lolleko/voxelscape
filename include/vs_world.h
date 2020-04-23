#pragma once

#include <map>

#include "vs_block.h"

class VSChunk;

class VSWorld
{
public:
    const VSBlockData* getBlockData(short ID);

private:
    std::map<VSBlockID, VSBlockData*> blockIDtoBlockData;

    std::vector<VSChunk*> loadedChunks;
    std::vector<VSChunk*> activeChunks;
};