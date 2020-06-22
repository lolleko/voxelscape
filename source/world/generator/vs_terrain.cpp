#include "world/generator/vs_terrain.h"
#include <glm/fwd.hpp>
#include <random>
#include <vector>
#include "world/generator/vs_heightmap.h"
#include "world/vs_block.h"
#include "world/vs_chunk_manager.h"

#include <iostream>
namespace VSTerrainGeneration
{
    void buildBiomes(VSWorld* world)
    {
        auto chunkManager = world->getChunkManager();
        glm::ivec3 worldSize = chunkManager->getWorldSize();
        glm::ivec3 worldSizeHalf = worldSize / 2;
        VSHeightmap flatHM = VSHeightmap(42, worldSizeHalf.y, 2, 0.001F, 1.F, 4.F, 0.25F);
        VSHeightmap mountainHM = VSHeightmap(42, worldSize.y, 2, 0.01F, worldSize.y, 4.F, 0.125F);

        int numBiomes = 30;
        VSHeightmap biomeMap = VSHeightmap(42, numBiomes, 1, 0.03F, 1.F, 4.F, 0.125F);

        std::random_device rd;   // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(0, 300);  // For tree map

        // int grassLine = worldSize.y / 2;
        // int sandLine = -worldSize.y / 4;
        // int waterLine = -worldSize.y / 3;

        for (int x = -worldSizeHalf.x; x < worldSizeHalf.x; x++)
        {
            for (int z = -worldSizeHalf.z; z < worldSizeHalf.z; z++)
            {
                int biome = biomeMap.getVoxelHeight(x, z);
                int height = flatHM.getVoxelHeight(x, z);
                int blockID = 3;
                if (biome > numBiomes / 2)
                {
                    // Interpolate
                    height = ((biome - numBiomes / 2) * (mountainHM.getVoxelHeight(x, z) + worldSize.y / 4) + (numBiomes - (biome - numBiomes / 2)) * flatHM.getVoxelHeight(x, z)) / numBiomes;
                    blockID = 1;
                }

                // if (height > grassLine)
                // {
                //     // Stone
                //     blockID = 1;
                // }
                // else if (height > sandLine)
                // {
                //     // Grass
                //     blockID = 3;
                // }
                // else if (height > waterLine)
                // {
                //     // Sand
                //     blockID = 5;
                // }
                // else
                // {
                //     // Water for now
                //     blockID = 2;
                //     height = waterLine;
                // }

                for (int y = -worldSizeHalf.y; y < height; y++)
                {
                    chunkManager->setBlock({x, y, z}, blockID);
                }

                // int tree = dis(gen);
                // if (tree == 0)
                // {
                //     if (height < 2 * worldSize.y / 3 && height > worldSize.y / 4)
                //     {
                //         if (x > -worldSizeHalf.x + 1 && z > -worldSizeHalf.z + 1 &&
                //             x < worldSizeHalf.x - 3 && z < worldSizeHalf.z - 3)
                //         {
                //             treeAt(world, x, height - worldSizeHalf.y, z);
                //         }
                //     }
                // }
            }
        }
    }

    void buildStandard(VSWorld* world)
    {
        auto chunkManager = world->getChunkManager();
        glm::ivec3 worldSize = chunkManager->getWorldSize();
        glm::ivec3 worldSizeHalf = worldSize / 2;
        VSHeightmap hm = VSHeightmap(42, worldSize.y, 2, 0.01F, worldSize.y, 4.F, 0.125F);

        std::random_device rd;   // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(0, 300);  // For tree map

        for (int x = -worldSizeHalf.x; x < worldSizeHalf.x; x++)
        {
            for (int z = -worldSizeHalf.z; z < worldSizeHalf.z; z++)
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

                for (int y = -worldSizeHalf.y; y < height; y++)
                {
                    chunkManager->setBlock({x, y, z}, blockID);
                }
                if (tree == 0)
                {
                    if (height < 2 * worldSize.y / 3 && height > worldSize.y / 4)
                    {
                        if (x > -worldSizeHalf.x + 1 && z > -worldSizeHalf.z + 1 &&
                            x < worldSizeHalf.x - 3 && z < worldSizeHalf.z - 3)
                        {
                            treeAt(world, x, height - worldSizeHalf.y, z);
                        }
                    }
                }
            }
        }
    }

    void buildMountains(VSWorld* world)
    {
        auto chunkManager = world->getChunkManager();
        glm::ivec3 worldSize = chunkManager->getWorldSize();
        glm::ivec3 worldSizeHalf = worldSize / 2;
        VSHeightmap hm = VSHeightmap(42, worldSize.y, 4, 0.02F, worldSize.y, 1.F, 1.F);

        std::random_device rd;   // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(0, 300);  // For tree map

        for (int x = -worldSizeHalf.x; x < worldSizeHalf.x; x++)
        {
            for (int z = -worldSizeHalf.z; z < worldSizeHalf.z; z++)
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

                for (int y = -worldSizeHalf.y; y < height - worldSizeHalf.y; y++)
                {
                    chunkManager->setBlock({x, y, z}, blockID);
                }
                if (tree == 0)
                {
                    if (height < 2 * worldSize.y / 3 && height > worldSize.y / 4)
                    {
                        if (x > -worldSizeHalf.x + 1 && z > -worldSizeHalf.z + 1 &&
                            x < worldSizeHalf.x - 3 && z < worldSizeHalf.z - 3)
                        {
                            treeAt(world, x, height - worldSizeHalf.y, z);
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
        glm::ivec3 worldSizeHalf = worldSize / 2;
        VSHeightmap desert = VSHeightmap(42, worldSize.y, 4, 0.02F, 40.F, 0.5F, 1.F);

        std::random_device rd;   // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(0, 3000);  // For cactus map

        for (int x = -worldSizeHalf.x; x < worldSizeHalf.x; x++)
        {
            for (int z = -worldSizeHalf.z; z < worldSizeHalf.z; z++)
            {
                int height = desert.getVoxelHeight(x, z);

                int tree = dis(gen);  // treeMap.getVoxelHeight(x, z);
                int blockID = 5;      // sand

                for (int y = -worldSizeHalf.y; y < height - worldSizeHalf.y; y++)
                {
                    chunkManager->setBlock({x, y, z}, blockID);
                }
                if (tree == 0)
                {
                    if (height < 2 * worldSize.y / 3 && height > worldSize.y / 4)
                    {
                        if (x > -worldSizeHalf.x + 1 && z > -worldSizeHalf.z + 1 &&
                            x < worldSizeHalf.x - 3 && z < worldSizeHalf.z - 3)
                        {
                            cactusAt(world, x, height - worldSizeHalf.y, z);
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
}  // namespace VSTerrainGeneration