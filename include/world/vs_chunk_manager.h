#pragma once

#include <glad/glad.h>
#include <glm/gtx/component_wise.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <vector>
#include <array>
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
        struct VSVisibleBlockInfo
        {
            glm::vec3 locationWorldSpace;
            VSBlockID id;
        };

        std::vector<VSBlockID> blocks;

        std::vector<bool> blockVisibility;

        std::atomic<bool> bIsDirty;

        std::array<std::vector<VSVisibleBlockInfo>, 64> visibleBlockInfos;

        glm::mat4 modelMatrix = glm::mat4(1.F);
    };

public:
    VSChunkManager();

    VSBlockID getBlock(glm::ivec3 location) const;

    void setBlock(glm::ivec3 location, VSBlockID blockID);

    glm::ivec3 getWorldSize() const;

    void draw(VSWorld* world) override;

    void updateChunks();

    void setChunkDimensions(const glm::ivec3& inChunkSize, const glm::ivec2& inChunkCount);

    std::size_t getChunkBlockCount() const;

    std::size_t getTotalBlockCount() const;

    std::size_t getVisibleBlockCount() const;

    std::size_t getDrawnBlockCount() const;

    std::size_t getTotalChunkCount() const;

    std::size_t getDrawCallCount() const;

private:
    std::vector<VSChunk*> chunks;

    glm::ivec3 chunkSize{};

    glm::ivec2 chunkCount{};

    glm::ivec3 worldSize{};

    VSShader chunkShader = VSShader("Chunk");

    std::atomic<bool> bShouldReinitializeChunks = false;

    static constexpr auto faceCombinationCount = 64;

    std::array<VSVertexContext*, faceCombinationCount> vertexContexts;

    std::array<GLuint, faceCombinationCount> visibleBlockInfoBuffers;

    glm::mat4 frozenVPMatrix;

    std::uint32_t drawCallCount;

    std::uint32_t drawnBlockCount;

    void initializeChunks();

    VSChunk* createChunk() const;

    void deleteChunk(VSChunk* chunk);

    bool updateVisibleBlocks(std::size_t chunkIndex);

    std::uint8_t isBlockVisible(std::size_t chunkIndex, std::size_t blockIndex) const;

    std::uint8_t
    isCenterBlockVisible(std::size_t chunkIndex, const glm::ivec3& blockCoordinates) const;

    std::uint8_t
    isBorderBlockVisible(std::size_t chunkIndex, const glm::ivec3& blockCoordinates) const;

    std::size_t chunkCoordinatesToChunkIndex(const glm::ivec2& chunkCoordinates) const;

    glm::ivec2 chunkIndexToChunkCoordinates(std::size_t chunkIndex) const;

    std::size_t blockCoordinatesToBlockIndex(const glm::ivec3& chunkCoords) const;

    glm::ivec3 blockIndexToBlockCoordinates(std::size_t blockIndex) const;

    glm::ivec2 worldCoordinatesToChunkCoordinates(const glm::ivec3& worldCoords) const;

    std::tuple<std::size_t, std::size_t>
    worldCoordinatesToChunkAndBlockIndex(const glm::ivec3& worldCoords) const;

    std::tuple<glm::ivec2, std::size_t>
    worldCoordinatesToChunkCoordinatesAndBlockIndex(const glm::ivec3& worldCoords) const;

    glm::ivec3
    blockCoordinatesToWorldCoordinates(std::size_t chunkIndex, const glm::ivec3& blockCoords) const;
};