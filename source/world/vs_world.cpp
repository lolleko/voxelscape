#include "world/vs_world.h"

#include <cmath>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <limits>
#include <memory>

#include "core/vs_camera.h"
#include "core/vs_fpcameracontroller.h"
#include "core/vs_debug_draw.h"
#include "world/vs_block.h"
#include "world/vs_chunk_manager.h"

VSWorld::VSWorld()
{
    camera = new VSCamera(glm::vec3(0.0F, 30.0F, 0.0F));
    cameraController = new VSFPCameraController(camera, this);
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

glm::ivec3 VSWorld::intersectRayWithBlock(glm::vec3 rayOrigin, glm::vec3 rayDirection)
{
    // Map to block coordinates
    rayOrigin += getChunkManager()->getWorldSize() / 2;

    rayDirection = glm::normalize(rayDirection);
    int i = 0;
    glm::ivec3 size = getChunkManager()->getWorldSize();
    glm::ivec3 location(std::floor(rayOrigin.x), std::floor(rayOrigin.y), std::floor(rayOrigin.z));
    glm::ivec3 previous(location);

    // Voxel traversal algorithm
    // Direction of increments
    float stepX = (rayDirection.x >= 0) ? 1 : -1;
    float stepY = (rayDirection.y >= 0) ? 1 : -1;
    float stepZ = (rayDirection.z >= 0) ? 1 : -1;

    // Distance to nearest voxel boundaries
    float nextVoxelX =
        location.x + stepX;  // times bin size if we ever decide to have different size than one
    float nextVoxelY = location.y + stepY;
    float nextVoxelZ = location.z + stepZ;

    // The value of t at which the ray crosses the first vertical voxel boundary
    float tMaxX =
        (rayDirection.x != 0) ? (nextVoxelX - rayOrigin.x) / rayDirection.x : std::numeric_limits<float>::max();
    float tMaxY =
        (rayDirection.y != 0) ? (nextVoxelY - rayOrigin.y) / rayDirection.y : std::numeric_limits<float>::max();
    float tMaxZ =
        (rayDirection.z != 0) ? (nextVoxelZ - rayOrigin.z) / rayDirection.z : std::numeric_limits<float>::max();

    float tDeltaX = (rayDirection.x != 0) ? stepX / rayDirection.x : std::numeric_limits<float>::max();
    float tDeltaY = (rayDirection.y != 0) ? stepY / rayDirection.y : std::numeric_limits<float>::max();
    float tDeltaZ = (rayDirection.z != 0) ? stepZ / rayDirection.z : std::numeric_limits<float>::max();

    while (i++ < 100)
    {
        location = glm::ivec3(std::floor(rayOrigin.x), std::floor(rayOrigin.y), std::floor(rayOrigin.z));
        if (location.x >= size.x || location.y >= size.y || location.z >= size.z)
        {
            // do nothing
        }
        else if (location.x < 0 || location.y < 0 || location.z < 0)
        {
            // do nothing
        }
        else
        {
            VSBlockID blockID = getChunkManager()->getBlock(location);
            if (blockID != 0)
            {
                // back to world coordinates
                return (previous - getChunkManager()->getWorldSize() / 2);
            }
        }

        previous = location;
        if (tMaxX < tMaxY)
        {
            if (tMaxX < tMaxZ)
            {
                rayOrigin.x += stepX;
                tMaxX += tDeltaX;
            }
            else
            {
                rayOrigin.z += stepZ;
                tMaxZ += tDeltaZ;
            }
        }
        else
        {
            if (tMaxY < tMaxZ)
            {
                rayOrigin.y += stepY;
                tMaxY += tDeltaY;
            }
            else
            {
                rayOrigin.z += stepZ;
                tMaxZ += tDeltaZ;
            }
        }
    }

    return glm::vec3();
}

glm::vec3 VSWorld::getDirectLightPos() const
{
    return directLightPos;
}

void VSWorld::setDirectLightPos(const glm::vec3& newDirectLightPos)
{
    directLightPos = newDirectLightPos;
}

glm::vec3 VSWorld::getDirectLightColor() const
{
    return {1.F, 1.F, 1.F};
}

VSChunkManager* VSWorld::getChunkManager() const
{
    return chunkManager;
}

VSDebugDraw* VSWorld::getDebugDraw() const
{
    return debugDraw;
}
