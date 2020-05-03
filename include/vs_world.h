#pragma once

#include <map>
#include <memory>

#include "vs_block.h"
#include "vs_camera.h"
#include "vs_drawable.h"
#include "vs_shader.h"

class VSChunk;

class VSWorld : public IVSDrawable
{
public:
    VSWorld();

    const VSBlockData* getBlockData(short ID);

    void addDrawable(std::shared_ptr<IVSDrawable> drawable, std::shared_ptr<VSShader> shader);

    void draw(std::shared_ptr<VSWorld> world, std::shared_ptr<VSShader> shader) const override;

    std::shared_ptr<VSCamera> getCamera() const;

    glm::vec3 getDirectLightPos() const;

    glm::vec3 getDirectLightColor() const;

private:
    std::map<VSBlockID, VSBlockData*> blockIDtoBlockData;

    std::vector<VSChunk*> loadedChunks;
    std::vector<VSChunk*> activeChunks;

    std::shared_ptr<VSCamera> camera;

    std::map<std::shared_ptr<IVSDrawable>, std::shared_ptr<VSShader>> drawables;
};