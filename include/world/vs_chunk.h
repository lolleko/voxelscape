#pragma once

#include <glad/glad.h>

#include <vector>
#include <map>
#include <atomic>
#include "renderer/vs_drawable.h"
#include "world/vs_transformable.h"
#include "world/vs_block.h"

struct VSVertexContext;

class VSChunk : public IVSDrawable, IVSTransformable
{
public:
    // Size can only be set on construction
    VSChunk(const glm::ivec3& size, VSBlockID defaultID);

    ~VSChunk();

    void updateActiveBlocks();

    // This is very costly
    void clearBlockData();;

    void setBlock(glm::ivec3 location, VSBlockID blockID);

    std::uint64_t  getTotalBlockCount() const;

    std::uint64_t  getActiveBlockCount() const;

    glm::vec3 getSize() const;

    void draw(VSWorld* world, std::shared_ptr<VSShader> shader) const override;;

    glm::mat4 getModelMatrix() const override;

    void setModelMatrix(const glm::mat4& mat) override;

private:
    glm::ivec3 size;

    VSBlockID defaultID;

    VSVertexContext* vertexContext;

    GLuint activeBlocksInstanceBuffer;

    std::vector<glm::vec3> drawnBlocksOffsets;

    std::vector<VSBlockID> blocks;

    std::atomic<bool> bIsDirty;

    glm::mat4 modelMatrix = glm::mat4(1.f);

    void updateBlock(int blockIndex);

    glm::ivec3 blockIndexToBlockCoords(int blockIndex) const;

    glm::vec3 blockIndexToBlockCoordsFloat(int blockIndex) const;

    int blockCoordsToBlockIndex(const glm::ivec3& blockCoords) const;

    bool isBlockVisible(int blockIndex) const;

    bool isAtBorder(const glm::ivec3& blockCoords) const;
};