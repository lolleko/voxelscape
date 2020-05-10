#pragma once

#include <map>
#include <memory>

#include "renderer/vs_drawable.h"
#include "renderer/vs_shader.h"
#include "world/vs_block.h"

class VSCamera;
class VSCameraController;
class VSChunk;

class VSWorld : public IVSDrawable
{
public:
    VSWorld();

    void initializeChunks();

    const VSBlockData* getBlockData(short ID);

    void addDrawable(IVSDrawable* drawable, std::shared_ptr<VSShader> shader);

    void removeDrawable(IVSDrawable* drawable);

    void removeDrawableDontDelete(IVSDrawable* drawable);

    void draw(VSWorld* world, std::shared_ptr<VSShader> shader) const override;

    VSCamera* getCamera() const;

    VSCameraController* getCameraController() const;

    glm::vec3 getDirectLightPos() const;

    glm::vec3 getDirectLightColor() const;

    void setBlock(glm::ivec3 location, VSBlockID blockID);

    void updateActiveChunks();

    glm::ivec3 getWorldSize();

    int getTotalBlockCount() const;

    int getActiveBlockCount() const;

    const glm::ivec3& getChunkSize() const;

    void setChunkSize(const glm::ivec3& inChunkSize);

    const glm::ivec2& getChunkCount() const;

    void setChunkCount(const glm::ivec2& inChunkCount);

    void setShouldDrawBorderBlocks(bool state);

private:
    std::map<VSBlockID, VSBlockData*> blockIDtoBlockData;

    std::vector<VSChunk*> loadedChunks;
    std::vector<VSChunk*> activeChunks;

    glm::ivec3 chunkSize;

    std::shared_ptr<VSShader> chunkShader;

    glm::ivec2 chunkCount;

    VSCamera* camera;
    VSCameraController* cameraController;

    std::map<IVSDrawable*, std::shared_ptr<VSShader>> drawables;
};