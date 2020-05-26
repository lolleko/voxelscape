#pragma once

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

    [[nodiscard]] glm::vec3 getDirectLightPos() const;

    void setDirectLightPos(const glm::vec3& newDirectLightPos);

    [[nodiscard]] glm::vec3 getDirectLightColor() const;

    [[nodiscard]] VSChunkManager* getChunkManager() const;

    [[nodiscard]] VSDebugDraw* getDebugDraw() const;

private:
    VSCamera* camera;
    VSCameraController* cameraController;

    VSChunkManager* chunkManager;

    VSDebugDraw* debugDraw;

    std::set<IVSDrawable*> drawables;

    glm::vec3 directLightPos{500.f, 1000.f, 500.f};
};