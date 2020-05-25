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

glm::ivec3 VSWorld::intersectRayWithBlock(glm::vec3 ray_o, glm::vec3 ray_d)
{
    // Map to block coordinates
    ray_o += this->getChunkManager()->getWorldSize() / 2;

    ray_d = glm::normalize(ray_d);
    int i = 0;
    glm::ivec3 size = this->getChunkManager()->getWorldSize();
    glm::ivec3 location(std::floor(ray_o.x), std::floor(ray_o.y), std::floor(ray_o.z));
    glm::ivec3 previous(location);

    // Voxel traversal algorithm
    // Direction of increments
    float stepX = (ray_d.x >= 0) ? 1 : -1;
    float stepY = (ray_d.y >= 0) ? 1 : -1;
    float stepZ = (ray_d.z >= 0) ? 1 : -1;

    // Distance to nearest voxel boundaries
    float next_voxel_x =
        location.x + stepX;  // times bin size if we ever decide to have different size than one
    float next_voxel_y = location.y + stepY;
    float next_voxel_z = location.z + stepZ;

    // The value of t at which the ray crosses the first vertical voxel boundary
    float tMaxX =
        (ray_d.x != 0) ? (next_voxel_x - ray_o.x) / ray_d.x : std::numeric_limits<float>::max();
    float tMaxY =
        (ray_d.y != 0) ? (next_voxel_y - ray_o.y) / ray_d.y : std::numeric_limits<float>::max();
    float tMaxZ =
        (ray_d.z != 0) ? (next_voxel_z - ray_o.z) / ray_d.z : std::numeric_limits<float>::max();

    float tDeltaX = (ray_d.x != 0) ? stepX / ray_d.x : std::numeric_limits<float>::max();
    float tDeltaY = (ray_d.y != 0) ? stepY / ray_d.y : std::numeric_limits<float>::max();
    float tDeltaZ = (ray_d.z != 0) ? stepZ / ray_d.z : std::numeric_limits<float>::max();

    while (i++ < 100)
    {
        location = glm::ivec3(std::floor(ray_o.x), std::floor(ray_o.y), std::floor(ray_o.z));
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
                return (previous - this->getChunkManager()->getWorldSize() / 2);
            }
        }

        previous = location;
        if (tMaxX < tMaxY)
        {
            if (tMaxX < tMaxZ)
            {
                ray_o.x += stepX;
                tMaxX += tDeltaX;
            }
            else
            {
                ray_o.z += stepZ;
                tMaxZ += tDeltaZ;
            }
        }
        else
        {
            if (tMaxY < tMaxZ)
            {
                ray_o.y += stepY;
                tMaxY += tDeltaY;
            }
            else
            {
                ray_o.z += stepZ;
                tMaxZ += tDeltaZ;
            }
        }
    }

    return glm::vec3();
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
