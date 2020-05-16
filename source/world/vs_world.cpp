#include "world/vs_world.h"

#include <glm/fwd.hpp>
#include <memory>
#include <glm/ext/matrix_transform.hpp>
#include <numeric>
#include <cassert>

#include "core/vs_camera.h"
#include "core/vs_core.h"
#include "core/vs_fpcameracontroller.h"
#include "world/vs_chunk.h"

VSWorld::VSWorld()
{
    camera = new VSCamera(glm::vec3(0.0F, 30.0F, 0.0F));
    // Default is first person camera controller
    cameraController = std::move(new VSFPCameraController(camera));
    chunkSize = {32.f, 64.f, 32.f};
    chunkCount = {2, 2};
    // request chunk rebuild at first frame
    bShouldRebuildChunks = true;
}

void VSWorld::initializeChunks()
{
    assert(debug_isMainThread());
    bool expected = true;
    if (bShouldRebuildChunks.compare_exchange_weak(expected, false))
    {
        for (auto& chunk : activeChunks)
        {
            removeDrawable(chunk);
        }
        activeChunks.empty();
        activeChunks.resize(chunkCount.x * chunkCount.y);
        if (!chunkShader)
        {
            chunkShader = std::make_shared<VSShader>("Chunk");
        }
        for (int y = 0; y < chunkCount.x; y++)
        {
            for (int x = 0; x < chunkCount.x; x++)
            {
                auto newChunk = new VSChunk(chunkSize, 0);
                newChunk->setModelMatrix(glm::translate(
                    newChunk->getModelMatrix(), glm::vec3(chunkSize.x * x, 0.f, chunkSize.z * y)));
                activeChunks[y * chunkCount.x + x] = newChunk;
                addDrawable(newChunk, chunkShader);
            }
        }
    }
}

const VSBlockData* VSWorld::getBlockData(short ID)
{
    (void) ID;
    // TODO stub
    return nullptr;
}

void VSWorld::addDrawable(IVSDrawable* drawable, std::shared_ptr<VSShader> shader)
{
    drawables.insert({drawable, shader});
}

void VSWorld::removeDrawable(IVSDrawable* drawable)
{
    removeDrawableDontDelete(drawable);
    delete drawable;
}

void VSWorld::removeDrawableDontDelete(IVSDrawable* drawable)
{
    drawables.erase(drawable);
}

void VSWorld::draw(VSWorld* world, std::shared_ptr<VSShader> shader) const
{
    (void) shader;
    for (const auto& [drawable, drawableShader] : drawables)
    {
        drawable->draw(world, drawableShader);
    }
}

VSCamera* VSWorld::getCamera() const
{
    return camera;
}

VSCameraController* VSWorld::getCameraController() const
{
    return cameraController;
}

void VSWorld::setCameraController(VSCameraController* newCameraController) 
{
    cameraController = newCameraController;
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
    // TODO race condition if this is called while rebuilding chunks
    // TODO extract to share method
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
    assert(debug_isMainThread());
    initializeChunks();
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
    assert(debug_isMainThread());
    return std::accumulate(activeChunks.begin(), activeChunks.end(), 0, [](int acc, VSChunk* curr){
        return acc + curr->getActiveBlockCount();
    });
}

int VSWorld::getTotalBlockCount() const
{
    assert(debug_isMainThread());
    return std::accumulate(activeChunks.begin(), activeChunks.end(), 0, [](int acc, VSChunk* curr){
      return acc + curr->getTotalBlockCount();
    });
}

const glm::ivec3& VSWorld::getChunkSize() const
{
    return chunkSize;
}

const glm::ivec2& VSWorld::getChunkCount() const
{
    return chunkCount;
}

void VSWorld::setChunkDimensions(const glm::ivec3& inChunkSize, const glm::ivec2& inChunkCount)
{
    chunkSize = inChunkSize;
    chunkCount = inChunkCount;
    bShouldRebuildChunks = true;
}

void VSWorld::clearBlocks() 
{
    for (auto* chunk :activeChunks) {
        chunk->clearBlockData();
    }
}
