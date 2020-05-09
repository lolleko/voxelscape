#include "world/vs_world.h"
#include <glm/fwd.hpp>
#include <memory>
#include <glm/ext/matrix_transform.hpp>
#include <world/generator/vs_heightmap.h>
#include <numeric>

#include "core/vs_camera.h"
#include "core/vs_cameracontroller.h"
#include "world/vs_chunk.h"

VSWorld::VSWorld()
{
    camera = std::make_shared<VSCamera>(glm::vec3(0.0F, 30.0F, 0.0F));
    cameraController = std::make_shared<VSCameraController>(camera);
    chunkSize = {100.f, 40.f, 100.f};
    chunkCount = {2, 2};
}

void VSWorld::initializeChunks()
{
    activeChunks.resize(chunkCount.x * chunkCount.y);
    // TOOD refactor into separate function
    chunkShader = std::__1::make_shared<VSShader>("Chunk");
    for (int y = 0; y < chunkCount.x; y++)
    {
        for (int x = 0; x < chunkCount.x; x++)
        {
            auto newChunk = std::__1::make_shared<VSChunk>(chunkSize, 0);
            newChunk->setModelMatrix(glm::translate(
                newChunk->getModelMatrix(), glm::vec3(chunkSize.x * x, 0.f, chunkSize.z * y)));
            activeChunks[y * chunkCount.x + x] = newChunk;
            addDrawable(newChunk, chunkShader);
        }
    }

    VSHeightmap hm = VSHeightmap(42, chunkSize.y, 1, 0.02F, 4.F);
    for (int x = 0; x < getWorldSize().x; x++)
    {
        for (int z = 0; z < getWorldSize().z; z++)
        {
            for (int y = 0; y < hm.getVoxelHeight(x, z); y++)
            {
                setBlock({x, y, z}, 1);
            }
        }
    }
    updateActiveChunks();
}

const VSBlockData* VSWorld::getBlockData(short ID)
{
    // TODO stub
    return nullptr;
}

void VSWorld::addDrawable(std::shared_ptr<IVSDrawable> drawable, std::shared_ptr<VSShader> shader)
{
    drawables.insert({drawable, shader});
}

void VSWorld::draw(std::shared_ptr<VSWorld> world, std::shared_ptr<VSShader> shader) const
{
    for (const auto& [drawable, drawableShader] : drawables)
    {
        drawable->draw(world, drawableShader);
    }
}

std::shared_ptr<VSCamera> VSWorld::getCamera() const
{
    return camera;
}

std::shared_ptr<VSCameraController> VSWorld::getCameraController() const
{
    return cameraController;
}

glm::vec3 VSWorld::getDirectLightPos() const
{
    return glm::vec3(10000.f, 10000.f, 20000.f);
}

glm::vec3 VSWorld::getDirectLightColor() const
{
    return glm::vec3(1.f, 1.f, 1.f);
}

void VSWorld::setBlock(glm::ivec3 location, VSBlockID blockID)
{
    const glm::ivec2 chunkLocation = {location.x / chunkSize.x, location.z / chunkSize.z};
    const glm::ivec3 blockPosition = {
        location.x - chunkLocation.x * chunkSize.x,
        location.y,
        location.z - chunkLocation.y * chunkSize.z};
    activeChunks[chunkLocation.y * chunkCount.x + chunkLocation.x]->setBlock(
        blockPosition, blockID);
}

void VSWorld::updateActiveChunks()
{
    for (auto chunk : activeChunks)
    {
        chunk->updateActiveBlocks();
    }
}

glm::ivec3 VSWorld::getWorldSize()
{
    return {chunkCount.x * chunkSize.x, chunkSize.y, chunkCount.y * chunkSize.z};
}

int VSWorld::getActiveBlockCount() const
{
    return std::accumulate(activeChunks.begin(), activeChunks.end(), 0, [](int acc, std::shared_ptr<VSChunk> curr){
        return acc + curr->getActiveBlockCount();
    });
}
int VSWorld::getTotalBlockCount() const
{
    return std::accumulate(activeChunks.begin(), activeChunks.end(), 0, [](int acc, std::shared_ptr<VSChunk> curr){
      return acc + curr->getTotalBlockCount();
    });
}
