#include "world/vs_world.h"

#include <memory>

#include "core/vs_camera.h"
#include "core/vs_cameracontroller.h"
#include "core/vs_debug_draw.h"
#include "world/vs_chunk_manager.h"


VSWorld::VSWorld()
{
    camera = new VSCamera(glm::vec3(0.0F, 30.0F, 0.0F));
    cameraController = new VSCameraController(camera);
    chunkManager = new VSChunkManager();
    debugDraw = new VSDebugDraw();
    addDrawable(debugDraw);
}

const VSBlockData* VSWorld::getBlockData(short ID)
{
    (void) ID;
    // TODO stub
    return nullptr;
}

void VSWorld::addDrawable(IVSDrawable* drawable)
{
    drawables.insert(drawable);
}

void VSWorld::removeDrawable(IVSDrawable* drawable)
{
    drawables.erase(drawable);
    delete drawable;
}

void VSWorld::update()
{
    chunkManager->updateChunks();
}

void VSWorld::draw(VSWorld* world) const
{
    for (const auto* drawable : drawables)
    {
        drawable->draw(world);
    }

    chunkManager->draw(world);
}

VSCamera* VSWorld::getCamera() const
{
    return camera;
}

VSCameraController* VSWorld::getCameraController() const
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

VSChunkManager* VSWorld::getChunkManager() const {
    return chunkManager;
}

VSDebugDraw* VSWorld::getDebugDraw() const
{
    return debugDraw;
}
