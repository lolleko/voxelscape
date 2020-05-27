#pragma once

#include <glm/fwd.hpp>
#include <set>
#include <memory>

#include "renderer/vs_drawable.h"
#include "renderer/vs_shader.h"
#include "world/vs_block.h"

class VSCamera;
class VSCameraController;
class VSChunkManager;
class VSDebugDraw;

class VSWorld : public IVSDrawable
{
public:
    VSWorld();

    void addDrawable(IVSDrawable* drawable);

    void removeDrawable(IVSDrawable* drawable);

    void update();

    void draw(VSWorld* world) override;

    [[nodiscard]] VSCamera* getCamera() const;

    [[nodiscard]] VSCameraController* getCameraController() const;

    void setCameraController(VSCameraController* newCameraController);

    /*
     * Intersect ray with world to find the block coordinates where the ray first hits a block.
     * Transforms input to block data to calculate intersections ans transforms back to world
     * coordinates before returning the intersection. For now we return the block coordinates before
     * the intersection to place a new block in the editor.
     */
    [[nodiscard]] glm::ivec3 intersectRayWithBlock(glm::vec3 rayOrigin, glm::vec3 rayDirection);

    [[nodiscard]] glm::vec3 getDirectLightPos() const;

    [[nodiscard]] glm::vec3 getDirectLightColor() const;

    [[nodiscard]] VSChunkManager* getChunkManager() const;

    [[nodiscard]] VSDebugDraw* getDebugDraw() const;

private:
    VSCamera* camera;
    VSCameraController* cameraController;

    VSChunkManager* chunkManager;

    VSDebugDraw* debugDraw;

    std::set<IVSDrawable*> drawables;
};