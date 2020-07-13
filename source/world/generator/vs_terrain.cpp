#include "world/generator/vs_terrain.h"
#include <glm/fwd.hpp>
#include <glm/gtx/easing.hpp>
#include <random>
#include <vector>
#include "ui/vs_parser.h"
#include "world/generator/vs_heightmap.h"
#include "world/vs_block.h"
#include "world/vs_chunk_manager.h"
#include "world/vs_world.h"

namespace VSTerrainGeneration
{
    void buildStandard(VSWorld* world)
    {
        auto chunkManager = world->getChunkManager();
        glm::ivec3 worldSize = chunkManager->getWorldSize();
        glm::ivec3 worldSizeHalf = worldSize / 2;
        VSHeightmap flatHM = VSHeightmap(worldSize.y / 4, 3, 0.005F, worldSize.y / 4, 2.F, 0.5F);
        VSHeightmap mountainHM =
            VSHeightmap(worldSize.y / 2, 2, 0.02F, worldSize.y / 2, 2.F, 0.125F);

        int numBiomes = 1000;
        VSHeightmap biomeMap = VSHeightmap(numBiomes, 1, 0.005F, 1.F, 2.F, 0.125F);

        std::random_device rd;   // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(0, 1000);  // For tree map
        std::uniform_int_distribution<> disEdge(0, 1);

        // load tree models
        const auto smallBirch =
            VSParser::readBuildFromFile("resources/trees/small_birchtree/blocks.json");
        const auto largeBirch =
            VSParser::readBuildFromFile("resources/trees/large_birchtree/blocks.json");

        int stoneLine = worldSize.y / 2;
        int grassLine = worldSize.y / 3;
        int waterLine = worldSize.y / 16;
        int sandLine = waterLine + 1;

        for (int x = -worldSizeHalf.x; x < worldSizeHalf.x; x++)
        {
            for (int z = -worldSizeHalf.z; z < worldSizeHalf.z; z++)
            {
                int biome = biomeMap.getVoxelHeight(x, z);
                int height = flatHM.getVoxelHeight(x, z);
                int mountainHeight = mountainHM.getVoxelHeight(x, z) + worldSizeHalf.y;

                // interpolate
                float weight = glm::quarticEaseIn((float)biome / numBiomes);
                height = ((1 - weight) * height + (weight)*mountainHeight);

                int blockID = 3;

                if (height > stoneLine + disEdge(gen))
                {
                    // Snow
                    blockID = 9;
                }
                else if (height > grassLine)
                {
                    // Stone
                    blockID = 1;
                }
                else if (height > sandLine)
                {
                    // Grass
                    blockID = 3;
                }
                else if (height > waterLine)
                {
                    // Sand
                    blockID = 5;
                }
                else
                {
                    // Water for now
                    blockID = 2;
                    height = waterLine;
                }

                for (int y = -worldSizeHalf.y; y < height - worldSizeHalf.y; y++)
                {
                    chunkManager->setBlock({x, y, z}, blockID);
                }

                int tree = dis(gen);
                if (tree == 0)
                {
                    if (height < stoneLine && height > sandLine)
                    {
                        if (x > -worldSizeHalf.x + 1 && z > -worldSizeHalf.z + 1 &&
                            x < worldSizeHalf.x - 3 && z < worldSizeHalf.z - 3)
                        {
                            treeAt(world, x, height - worldSizeHalf.y, z);
                        }
                    }
                }
                else if (tree == 1)
                {
                    if (height < grassLine && height > sandLine)
                    {
                        placeModelAt(world, smallBirch, x, height - worldSizeHalf.y, z);
                    }
                }
                else if (tree == 2)
                {
                    if (height < grassLine && height > sandLine)
                    {
                        placeModelAt(world, largeBirch, x, height - worldSizeHalf.y, z);
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
        VSHeightmap hm = VSHeightmap(worldSize.y, 4, 0.01F, worldSize.y, 1.F, 0.5F);

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
        VSHeightmap desert = VSHeightmap(worldSize.y / 10, 2, 0.02F, 10.F, 0.5F, 2.F);

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
                    if (x > -worldSizeHalf.x + 1 && z > -worldSizeHalf.z + 1 &&
                        x < worldSizeHalf.x - 3 && z < worldSizeHalf.z - 3)
                    {
                        cactusAt(world, x, height - worldSizeHalf.y, z);
                    }
                }
            }
        }
    }

    void buildEditorPlane(VSWorld* world)
    {
        auto chunkManager = world->getChunkManager();
        glm::ivec3 worldSize = chunkManager->getWorldSize();
        glm::ivec3 worldSizeHalf = worldSize / 2;

        for (int x = -worldSizeHalf.x; x < worldSizeHalf.x; x++)
        {
            for (int z = -worldSizeHalf.z; z < worldSizeHalf.z; z++)
            {
                chunkManager->setBlock({x, 0, z}, 1);
            }
        }
    }

    void placeModelAt(VSWorld* world, VSChunkManager::VSBuildingData build, int i, int j, int k)
    {
        auto chunkManager = world->getChunkManager();

        const glm::vec2 boundsXZ = {(glm::vec3(build.buildSize) / 2.F).x,
                                    (glm::vec3(build.buildSize) / 2.F).z};

        for (int x = 0; x < build.buildSize.x; x++)
        {
            for (int y = 0; y < build.buildSize.y; y++)
            {
                for (int z = 0; z < build.buildSize.z; z++)
                {
                    if (chunkManager->isLocationInBounds(
                            glm::vec3{x, y, z} + glm::vec3{i, j, k} -
                            glm::vec3(-boundsXZ.x, 0, -boundsXZ.y)))
                    {
                        chunkManager->setBlock(
                            glm::vec3{x, y, z} + glm::vec3{i, j, k} -
                                glm::vec3(-boundsXZ.x, 0, -boundsXZ.y),
                            build.blocks
                                [x + y * build.buildSize.x +
                                 z * build.buildSize.x * build.buildSize.y]);
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

    void birchtreeAt(VSWorld* world, int x, int y, int z)
    {
        auto chunkManager = world->getChunkManager();
        chunkManager->setBlock({x, y, z}, 22);
        chunkManager->setBlock({x, y + 1, z}, 22);
        chunkManager->setBlock({x, y + 2, z}, 22);
        chunkManager->setBlock({x, y + 3, z}, 22);
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
        chunkManager->setBlock({x, y, z}, 8);
        chunkManager->setBlock({x, y + 1, z}, 8);
        chunkManager->setBlock({x, y + 2, z}, 8);
        chunkManager->setBlock({x, y + 3, z}, 8);
        chunkManager->setBlock({x, y + 4, z}, 8);
        chunkManager->setBlock({x, y + 5, z}, 8);
        chunkManager->setBlock({x, y + 1, z + 1}, 8);
        chunkManager->setBlock({x, y + 1, z + 2}, 8);
        chunkManager->setBlock({x, y + 2, z + 2}, 8);
        chunkManager->setBlock({x, y + 2, z - 1}, 8);
        chunkManager->setBlock({x, y + 2, z - 2}, 8);
        chunkManager->setBlock({x, y + 3, z - 2}, 8);
    }
}  // namespace VSTerrainGeneration