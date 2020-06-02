#include "world/generator/vs_terrain.h"
#include <glm/fwd.hpp>
#include <random>
#include <vector>
#include "world/generator/vs_heightmap.h"
#include "world/vs_block.h"
#include "world/vs_chunk_manager.h"

namespace VSTerrainGeneration
{
    void buildTerrain(VSWorld* world)
    {
        // int biomeCount = 2; /* Flatland, Mountains, Forrest */
        auto chunkManager = world->getChunkManager();
        glm::ivec3 worldSize = chunkManager->getWorldSize();
        // VSHeightmap biomeMap = VSHeightmap(42, biomeCount, 1, 0.01F, 1.F);

        // VSHeightmap flatland = VSHeightmap(42, worldSize.y, 4, 0.02F, 40.F, 0.5F, 1.F);
        // VSHeightmap mountains = VSHeightmap(42, worldSize.y, 4, 0.02F, 1.F);
        VSHeightmap desert = VSHeightmap(42, worldSize.y, 4, 0.02F, 40.F, 0.5F, 1.F);
        bool desertMode = true;
        VSHeightmap hm = VSHeightmap(42, worldSize.y, 4, 0.02F, worldSize.y, 1.F, 1.F);
        // std::vector<VSHeightmap> biomeMaps = {flatland, mountains, hm};

        std::random_device rd;   // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(0, 3000);  // For tree map

        std::cout << worldSize.y << std::endl;
        for (int x = 0; x < worldSize.x; x++)
        {
            for (int z = 0; z < worldSize.z; z++)
            {
                // int biome = biomeMap.getVoxelHeight(x, z);
                int height = hm.getVoxelHeight(x, z);
                if (desertMode)
                {
                    height = desert.getVoxelHeight(x, z);
                }
                int tree = dis(gen);  // treeMap.getVoxelHeight(x, z);
                int blockID = 0;
                if (desertMode)
                {
                    // Sand
                    blockID = 5;
                }
                else
                {
                    if (height > worldSize.y / 2)
                    {
                        // Stone
                        blockID = 1;
                    }
                    else if (height > worldSize.y / 4)
                    {
                        // Grass
                        blockID = 3;
                    }
                    else if (height > worldSize.y / 5)
                    {
                        // Sand
                        blockID = 5;
                    }
                    else
                    {
                        // Water for now
                        blockID = 2;
                        height = worldSize.y / 5;
                    }
                }

                for (int y = 0; y < height; y++)
                {
                    chunkManager->setBlock({x, y, z}, blockID);
                }
                if (tree == 50)
                {
                    if (height < 2 * worldSize.y / 3 && height > worldSize.y / 4)
                    {
                        if (x > 1 && z > 1 && x < worldSize.x - 3 && z < worldSize.z - 3)
                        {
                            if (desertMode)
                            {
                                cactusAt(world, x, height, z);
                            }
                            else
                            {
                                treeAt(world, x, height, z);
                            }
                        }
                    }
                }
            }
        }

        // build tree
        // treeAt(world, 10, hm.getVoxelHeight(10, 10), 10);

        // printMap();
    }

    void buildTaiga(VSWorld* world)
    {
        auto chunkManager = world->getChunkManager();
        glm::ivec3 worldSize = chunkManager->getWorldSize();
        VSHeightmap hm = VSHeightmap(42, worldSize.y, 4, 0.02F, worldSize.y, 1.F, 1.F);

        std::random_device rd;   // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(0, 300);  // For tree map

        for (int x = 0; x < worldSize.x; x++)
        {
            for (int z = 0; z < worldSize.z; z++)
            {
                int height = hm.getVoxelHeight(x, z);
                int tree = dis(gen);
                int blockID = 0;
                if (height > 2 * worldSize.y / 3)
                {
                    // Stone
                    blockID = 1;
                }
                else if (height > worldSize.y / 4)
                {
                    // Grass
                    blockID = 3;
                }
                else if (height > worldSize.y / 5)
                {
                    // Sand
                    blockID = 5;
                }
                else
                {
                    // Water for now
                    blockID = 2;
                    height = worldSize.y / 5;
                }

                for (int y = 0; y < height; y++)
                {
                    chunkManager->setBlock({x, y, z}, blockID);
                }
                if (tree == 0)
                {
                    if (height < 2 * worldSize.y / 3 && height > worldSize.y / 4)
                    {
                        if (x > 1 && z > 1 && x < worldSize.x - 3 && z < worldSize.z - 3)
                        {
                            treeAt(world, x, height, z);
                        }
                    }
                }
            }
        }
    }

    void buildDesert(VSWorld* world)
    {
        auto chunkManager = world->getChunkManager();
        glm::ivec3 worldSize = chunkManager->getWorldSize();
        VSHeightmap desert = VSHeightmap(42, worldSize.y, 4, 0.02F, 40.F, 0.5F, 1.F);

        std::random_device rd;   // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(0, 3000);  // For cactus map

        for (int x = 0; x < worldSize.x; x++)
        {
            for (int z = 0; z < worldSize.z; z++)
            {
                int height = desert.getVoxelHeight(x, z);

                int tree = dis(gen);  // treeMap.getVoxelHeight(x, z);
                int blockID = 5;      // sand

                for (int y = 0; y < height; y++)
                {
                    chunkManager->setBlock({x, y, z}, blockID);
                }
                if (tree == 0)
                {
                    if (height < 2 * worldSize.y / 3 && height > worldSize.y / 4)
                    {
                        if (x > 1 && z > 1 && x < worldSize.x - 3 && z < worldSize.z - 3)
                        {
                            cactusAt(world, x, height, z);
                        }
                    }
                }
            }
        }
    }

    void treeAt(VSWorld* world, int x, int y, int z)
    {
        auto chunkManager = world->getChunkManager();
        chunkManager->setBlock({x, y, z}, 4);
        chunkManager->setBlock({x, y + 1, z}, 4);
        chunkManager->setBlock({x, y + 2, z}, 4);
        chunkManager->setBlock({x, y + 3, z}, 4);
        chunkManager->setBlock({x + 1, y + 3, z}, 6);
        chunkManager->setBlock({x - 1, y + 3, z}, 6);
        chunkManager->setBlock({x, y + 3, z + 1}, 6);
        chunkManager->setBlock({x, y + 3, z - 1}, 6);
        chunkManager->setBlock({x + 1, y + 3, z + 1}, 6);
        chunkManager->setBlock({x - 1, y + 3, z - 1}, 6);
        chunkManager->setBlock({x + 1, y + 3, z - 1}, 6);
        chunkManager->setBlock({x - 1, y + 3, z + 1}, 6);
        chunkManager->setBlock({x, y + 4, z}, 6);
    }

    void cactusAt(VSWorld* world, int x, int y, int z)
    {
        auto chunkManager = world->getChunkManager();
        chunkManager->setBlock({x, y, z}, 6);
        chunkManager->setBlock({x, y + 1, z}, 6);
        chunkManager->setBlock({x, y + 2, z}, 6);
        chunkManager->setBlock({x, y + 3, z}, 6);
        chunkManager->setBlock({x, y + 4, z}, 6);
        chunkManager->setBlock({x, y + 5, z}, 6);
        chunkManager->setBlock({x, y + 1, z + 1}, 6);
        chunkManager->setBlock({x, y + 1, z + 2}, 6);
        chunkManager->setBlock({x, y + 2, z + 2}, 6);
        chunkManager->setBlock({x, y + 2, z - 1}, 6);
        chunkManager->setBlock({x, y + 2, z - 2}, 6);
        chunkManager->setBlock({x, y + 3, z - 2}, 6);
    }

    void printMap()
    {
        int biomeCount = 3;
        VSHeightmap hm = VSHeightmap(42, biomeCount, 1, 10000.F, 1.F);
        for (int x = 0; x < 80; x++)
        {
            for (int y = 0; y < 50; y++)
            {
                std::cout << hm.getVoxelHeight(x, y) << " ";
            }
            std::cout << std::endl;
        }
    }
}  // namespace VSTerrainGeneration