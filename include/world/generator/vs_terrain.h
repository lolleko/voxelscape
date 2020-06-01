#pragma once

#include "world/generator/vs_heightmap.h"
#include "world/vs_world.h"

namespace VSTerrainGeneration
{
    struct Config {
        
    };

    void buildTerrain(VSWorld* world);
    void buildDesert(VSWorld* world);
    void buildTaiga(VSWorld* world);

    void treeAt(VSWorld* world, int x, int y, int z);
    void cactusAt(VSWorld* world, int x, int y, int z);

    void printMap();
};