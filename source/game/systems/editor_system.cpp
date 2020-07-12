#include "game/systems/editor_system.h"
#include <glm/fwd.hpp>
#include "game/components/bounds.h"
#include "ui/vs_parser.h"

void updateEditorSystem(entt::registry& mainRegistry)
{
    const auto& inputs = mainRegistry.ctx<Inputs>();

    const auto& worldContext = mainRegistry.ctx<WorldContext>();

    auto& uiContext = mainRegistry.ctx<UIContext>();

    if (inputs.mouseTrace.bHasHit)
    {
        const auto mouseLocation = inputs.mouseTrace.hitLocation;
        // Check if block is placed in bounds
        if (!worldContext.bounds.isLocationInside(mouseLocation))
        {
            // Do nothing
        }
        else
        {
            // For debugging
            Bounds bounds{{0, 0, 0}, uiContext.brushSize};

            glm::vec3 discreteMouse{std::floor(mouseLocation.x - std::floor(bounds.getCenter().x)),
                                    std::floor(mouseLocation.y),
                                    std::floor(mouseLocation.z - std::floor(bounds.getCenter().z))};

            worldContext.world->getDebugDraw()->drawBox(
                {discreteMouse, discreteMouse + bounds.max}, {255, 0, 0});

            if (inputs.rightButtonState == InputState::JustUp)
            {
                const auto low = discreteMouse + bounds.min;
                const auto high = discreteMouse + bounds.max;

                for (int x = low.x; x < high.x; x++)
                {
                    for (int y = low.y; y < high.y; y++)
                    {
                        for (int z = low.z; z < high.z; z++)
                        {
                            worldContext.world->getChunkManager()->setBlock({x, y, z}, uiContext.editorSelectedBlockID + 1);
                        }
                    }
                }
            }
            else if (inputs.middleButtonState == InputState::JustUp)
            {
                worldContext.world->getChunkManager()->setBlock(
                    mouseLocation - 0.05F * inputs.mouseTrace.hitNormal, 0);
            }
        }
    }

    if (uiContext.bShouldSaveToFile)
    {
        const VSChunkManager::VSWorldData worldData =
            worldContext.world->getChunkManager()->getData();
        VSParser::writeToFile(worldData, uiContext.saveFilePath);
        uiContext.bShouldSaveToFile = false;
    }

    if (uiContext.bShouldLoadFromFile)
    {
        VSChunkManager::VSWorldData worldData = VSParser::readFromFile(uiContext.loadFilePath);
        worldContext.world->getChunkManager()->setWorldData(worldData);
        uiContext.bShouldLoadFromFile = false;
    }

    if (uiContext.bShouldSaveBuilding)
    {
        // Extract editor plane and save building
        const VSChunkManager::VSBuildingData buildData = extractBuildFromPlane(worldContext.world);
        VSParser::writeBuildToFile(buildData, uiContext.saveBuildingPath);
        uiContext.bShouldSaveBuilding = false;
    }
}

VSChunkManager::VSBuildingData extractBuildFromPlane(VSWorld* editorWorld)
{
    VSChunkManager::VSBuildingData buildData;
    const auto worldSize = editorWorld->getChunkManager()->getWorldSize();
    const auto worldSizeHalf = worldSize / 2;
    int xMin = worldSize.x;
    int xMax = -worldSize.x;
    int yMin = worldSize.y;
    int yMax = -worldSize.y;
    int zMin = worldSize.z;
    int zMax = -worldSize.z;

    for (int x = -worldSizeHalf.x; x < worldSizeHalf.x; x++)
    {
        for (int z = -worldSizeHalf.z; z < worldSizeHalf.z; z++)
        {
            for (int y = -worldSizeHalf.y; y < worldSizeHalf.y; y++)
            {
                int blockID = editorWorld->getChunkManager()->getBlock({x, y, z});
                if (blockID > 1 || (blockID != 0 && y != 0))
                {
                    xMin = std::min(xMin, x);
                    yMin = std::min(yMin, y);
                    zMin = std::min(zMin, z);
                    xMax = std::max(xMax, x);
                    yMax = std::max(yMax, y);
                    zMax = std::max(zMax, z);
                }
            }
        }
    }

    buildData.buildSize = {
        std::abs(xMax - xMin) + 1, std::abs(yMax - yMin) + 1, std::abs(zMax - zMin) + 1};

    for (int x = xMin; x <= xMax; x++)
    {
        for (int y = yMin; y <= yMax; y++)
        {
            for (int z = zMin; z <= zMax; z++)
            {
                buildData.blocks.emplace_back(editorWorld->getChunkManager()->getBlock({x, y, z}));
            }
        }
    }

    return buildData;
}