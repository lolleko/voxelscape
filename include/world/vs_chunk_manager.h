#pragma once

#include <glad/glad.h>
#include <atomic>
#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <vector>
#include <array>
#include <bitset>
#include <renderer/vs_shader.h>
#include <future>

#include "core/vs_core.h"

#include "renderer/vs_drawable.h"
#include "renderer/vs_vertex_context.h"

#include "world/vs_chunk_update.h"

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
            std::uint8_t lightLevel;
            std::uint32_t vc;
            std::uint32_t vd;
        };

        std::vector<VSBlockID> blocks;

        std::vector<bool> bIsBlockVisible;

        std::atomic<bool> bIsDirty;

        std::atomic<bool> bShouldRebuildShadows;

        std::array<std::vector<VSVisibleBlockInfo>, 64> visibleBlockInfos;

        glm::vec3 chunkLocation = glm::vec3(0.F);
    };

public:
    // This struct is used for serialization (load/save)
    struct VSWorldData
    {
        glm::ivec3 chunkSize;
        glm::ivec2 chunkCount;
        std::vector<VSBlockID> blocks;
    };

    struct VSBuildingData
    {
        glm::ivec3 buildSize;
        std::vector<VSBlockID> blocks;
    };

    struct VSTraceResult
    {
        bool bHasHit = false;
        glm::vec3 hitLocation = {};
        glm::vec3 hitNormal = {};
        VSBlockID blockID = VS_DEFAULT_BLOCK_ID;
    };

    VSChunkManager();

    VSBlockID getBlock(const glm::vec3& location) const;

    void setBlock(const glm::vec3& location, VSBlockID blockID);

    glm::ivec3 getWorldSize() const;

    void draw(VSWorld* world) override;

    void updateChunks();

    void setChunkDimensions(const glm::ivec3& inChunkSize, const glm::ivec2& inChunkCount);

    void setWorldData(const VSWorldData& worldData);

    std::size_t getChunkBlockCount() const;

    std::size_t getTotalBlockCount() const;

    std::size_t getVisibleBlockCount() const;

    std::size_t getDrawnBlockCount() const;

    std::size_t getTotalChunkCount() const;

    std::size_t getDrawCallCount() const;

    bool shouldReinitializeChunks() const;

    bool isLocationInBounds(const glm::ivec3& location) const;

    VSTraceResult lineTrace(const glm::vec3& start, const glm::vec3& end) const;

    // This method is used to retrieve the data to save a scene.
    [[nodiscard]] VSWorldData getData() const;

    void initFromData(const VSWorldData& data);

private:
    std::vector<VSChunk*> chunks;

    glm::ivec3 chunkSize{};

    glm::ivec2 chunkCount{};

    glm::ivec3 worldSize{};

    glm::ivec3 worldSizeHalf{};

    glm::ivec3 newChunkSize{};

    glm::ivec2 newChunkCount{};

    glm::ivec3 newWorldSize{};

    glm::ivec3 newWorldSizeHalf{};

    VSShader chunkShader = VSShader("Chunk");

    std::atomic<bool> bShouldReinitializeChunks = false;

    std::atomic<bool> bShouldInitializeFromData = false;

    VSWorldData worldDataFromFile;

    static constexpr auto faceCombinationCount = 64;

    std::array<VSVertexContext*, faceCombinationCount> vertexContexts;

    std::array<GLuint, faceCombinationCount> visibleBlockInfoBuffers;

    glm::mat4 frozenVPMatrix;

    std::uint32_t drawCallCount;

    std::uint32_t drawnBlockCount;

    GLuint spriteTexture;

    GLuint spriteTextureID;

    GLuint shadowTexture;

    GLuint shadowTextureID;

    using VSShadwoChunkUpdate = VSChunkUpdate<std::vector<float>>;

    std::map<VSChunk*, std::shared_ptr<VSShadwoChunkUpdate>> activeShadowBuildTasks;

    static inline auto maxShadowUpdateThreads = std::thread::hardware_concurrency();

    void initializeChunks();

    glm::ivec2 getChunkCount() const;

    VSChunk* createChunk() const;

    void deleteChunk(VSChunk* chunk);

    void updateShadows(std::size_t chunkIndex);

    std::vector<float> chunkUpdateShadow(
        const std::atomic<bool>& bShouldCancel,
        std::atomic<bool>& bIsReady,
        std::size_t chunkIndex) const;

    bool updateVisibleBlocks(std::size_t chunkIndex);

    std::uint8_t isBlockVisible(std::size_t chunkIndex, std::size_t blockIndex) const;

    std::uint8_t
    isCenterBlockVisible(std::size_t chunkIndex, const glm::ivec3& blockCoordinates) const;

    std::uint8_t
    isBorderBlockVisible(std::size_t chunkIndex, const glm::ivec3& blockCoordinates) const;

    bool isAtWorldBorder(const glm::ivec3& blockWorldCoordinates) const;

    std::tuple<std::uint32_t, std::uint32_t>
    getAdjacencyInformation(const glm::vec3& blockCoordinates) const;

    std::bitset<8> getAdjacencyInformationForFace(
        const glm::vec3& blockWorldCoordinates,
        const glm::vec3& faceDir) const;

    std::size_t chunkCoordinatesToChunkIndex(const glm::ivec2& chunkCoordinates) const;

    glm::ivec2 chunkIndexToChunkCoordinates(std::size_t chunkIndex) const;

    std::size_t blockCoordinatesToBlockIndex(const glm::ivec3& bloockCoords) const;

    glm::ivec3 blockIndexToBlockCoordinates(std::size_t blockIndex) const;

    glm::ivec2 worldCoordinatesToChunkCoordinates(const glm::ivec3& worldCoords) const;

    std::tuple<std::size_t, std::size_t>
    worldCoordinatesToChunkAndBlockIndex(const glm::ivec3& worldCoords) const;

    std::tuple<glm::ivec2, std::size_t>
    worldCoordinatesToChunkCoordinatesAndBlockIndex(const glm::ivec3& worldCoords) const;

    glm::ivec3
    blockCoordinatesToWorldCoordinates(std::size_t chunkIndex, const glm::ivec3& blockCoords) const;
};