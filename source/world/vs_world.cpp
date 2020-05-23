#include "world/vs_world.h"

#include <memory>

#include "core/vs_camera.h"
#include "core/vs_fpcameracontroller.h"
#include "core/vs_debug_draw.h"
#include "world/vs_chunk_manager.h"

VSWorld::VSWorld()
{
    camera = new VSCamera(glm::vec3(0.0F, 30.0F, 0.0F));
    cameraController = new VSFPCameraController(camera);
    chunkManager = new VSChunkManager();
    debugDraw = new VSDebugDraw();
    addDrawable(debugDraw);
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

void VSWorld::draw(VSWorld* world)
{
    for (auto* drawable : drawables)
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

void VSWorld::setCameraController(VSCameraController* newCameraController) 
{
    cameraController = newCameraController;
}

glm::vec3 VSWorld::getDirectLightPos() const
{
    return {10000.F, 10000.F, 20000.F};
}

glm::vec3 VSWorld::getDirectLightColor() const
{
    return glm::vec3(1.F, 1.F, 1.F);
}

VSChunkManager* VSWorld::getChunkManager() const
{
    return chunkManager;
}

VSDebugDraw* VSWorld::getDebugDraw() const
{
    return debugDraw;
}
