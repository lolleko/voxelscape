#pragma once

#include <glad/glad.h>

#include <vector>
#include "vs_drawable.h"
#include "vs_transformable.h"
#include "vs_block.h"

struct VSVertexContext;

class VSChunk : public IVSDrawable, IVSTransformable
{
public:
    // Size can only be set on construction
    VSChunk(const glm::vec3& size, VSBlockID defaultID);

    void updateActiveBlocks();;

    void setBlock(glm::ivec3 location, VSBlockID blockID);

    int getTotalBlockCount() const;

    int getActiveBlockCount() const;

    glm::vec3 getSize() const;

    void draw(std::shared_ptr<VSWorld> world, std::shared_ptr<VSShader> shader) const override;;

    glm::mat4 getModelMatrix() const override;

    void setShouldDrawBorderBlocks(bool state);

private:
    glm::vec3 size;

    VSBlockID defaultID;

    std::unique_ptr<VSVertexContext> vertexContext;

    GLuint activeBlocksInstanceBuffer;

    std::vector<glm::vec3> activeBlockOffsets;

    VSBlockID* blocks;

    bool bShouldDrawBorderBlocks = false;

    glm::ivec3 blockIndexToBlockCoords(int blockIndex) const;

    glm::vec3 blockIndexToBlockCoordsFloat(int blockIndex) const;

    int blockCoordsToBlockIndex(const glm::ivec3& blockCoords) const;

    bool isBlockVisible(int blockIndex);
};