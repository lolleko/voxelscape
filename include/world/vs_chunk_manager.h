#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>
#include <vector>
#include <glm/ext/matrix_transform.hpp>
#include <renderer/vs_shader.h>

#include "core/vs_core.h"

#include "renderer/vs_drawable.h"
#include "renderer/vs_vertex_context.h"

#include "vs_block.h"

struct VSVertexContext;

class VSShader;

class VSChunkManager : public IVSDrawable
{
    struct VSChunk
    {
        VSVertexContext* vertexContext;

        GLuint activeBlocksInstanceBuffer;

        std::vector<glm::vec3> drawnBlocksOffsets;

        std::vector<VSBlockID> blocks;

        std::vector<bool> blockVisibility;

        std::atomic<bool> bIsDirty;

        glm::mat4 modelMatrix = glm::mat4(1.f);
    };

public:
    VSChunkManager();

    VSBlockID getBlock(glm::ivec3 location) const;

    void setBlock(glm::ivec3 location, VSBlockID blockID);

    glm::ivec3 getWorldSize() const;

    void draw(VSWorld* world) const override;

    void updateChunks();;

    void setChunkDimensions(const glm::ivec3& inChunkSize, const glm::ivec2& inChunkCount);

private:
    std::vector<VSChunk*> chunks;

    glm::ivec3 chunkSize{};

    glm::ivec2 chunkCount{};

    glm::ivec3 worldSize;

    glm::ivec3 worldSizeHalf;

    VSShader chunkShader = VSShader("Chunk");

    std::atomic<bool> bShouldReinitializeChunks = false;

    void initializeChunks();

    VSChunk* createChunk();

    void deleteChunk(VSChunk* chunk);

    bool updateVisibleBlocks(std::size_t chunkIndex);

    bool isBlockVisible(std::size_t chunkIndex, std::size_t blockIndex) const;

    bool isCenterBlockVisible(std::size_t chunkIndex, const glm::ivec3& blockCoordinates) const;

    bool isBorderBlockVisible(std::size_t chunkIndex, const glm::ivec3& blockCoordinates) const;

    std::size_t chunkCoordinatesToChunkIndex(const glm::ivec2 chunkCoordinates) const;

    glm::ivec2 chunkIndexToChunkCoordinates(std::size_t chunkIndex) const;

    std::size_t blockCoordinatesToBlockIndex(const glm::ivec3& chunkCoords) const;;

    glm::ivec3 blockIndexToBlockCoordinates(std::size_t blockIndex) const;;

    glm::ivec2 worldCoordinatesToChunkCoordinates(const glm::ivec3& worldCoords) const;;

    std::tuple<std::size_t, std::size_t>
    worldCoordinatesToChunkAndBlockIndex(const glm::ivec3& worldCoords) const;;

    glm::ivec3
    blockCoordinatesToWorldCoordinates(std::size_t chunkIndex, const glm::ivec3& blockCoords) const;;
};