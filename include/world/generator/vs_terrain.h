#pragma once

#include "world/generator/vs_heightmap.h"
#include "world/vs_world.h"

namespace VSTerrainGeneration
{
    void buildTerrain(VSWorld* world);
    void buildDesert(VSWorld* world);
    void buildMountains(VSWorld* world);
    void buildStandard(VSWorld* world);
    void buildEditorPlane(VSWorld* world);

    void treeAt(VSWorld* world, int x, int y, int z);
    void cactusAt(VSWorld* world, int x, int y, int z);

    void printMap();
};