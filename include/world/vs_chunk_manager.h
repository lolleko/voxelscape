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
            std::uint32_t lightRight;
            std::uint32_t lightLeft;
            std::uint32_t lightTop;
            std::uint32_t lightBottom;
            std::uint32_t lightFront;
            std::uint32_t lightBack;
        };

        using VSVisibleBlockInfos = std::array<std::vector<VSVisibleBlockInfo>, 64>;

        std::vector<VSBlockID> blocks;

        std::vector<float> lightLevel;

        std::vector<bool> bIsBlockVisible;

        std::atomic<bool> bIsDirty;

        std::atomic<bool> bShouldRebuildShadows;

        VSVisibleBlockInfos visibleBlockInfos;

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

    void addEmission(const glm::vec3& location, float emission);

    glm::ivec3 getWorldSize() const;

    void draw(VSWorld* world) override;

    void updateChunks();

    [[nodiscard]] glm::vec3 getOrigin() const;

    void setOrigin(const glm::vec3& newOrigin);

    bool isFrustumCullingEnabled() const;

    void setIsFrustumCullingEnabled(bool state);

    glm::vec3 getColorOverride() const;

    void setColorOverride(const glm::vec3& newColorOverride);

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

    glm::vec3 origin{};

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

    bool bIsFrustumCullingEnabled = true;

    glm::vec3 colorOverride{1.F, 1.F, 1.F};

    VSWorldData worldDataFromFile;

    static constexpr auto faceCombinationCount = 64;

    std::array<VSVertexContext*, faceCombinationCount> vertexContexts;

    std::array<GLuint, faceCombinationCount> visibleBlockInfoBuffers;

    glm::mat4 frozenVPMatrix;
    glm::vec3 frozenCameraPos;

    std::uint32_t drawCallCount;

    std::uint32_t drawnBlockCount;

    GLuint spriteTexture;

    GLuint spriteTextureID;

    GLuint shadowTexture;

    GLuint shadowTextureID;

    using VSShadwoChunkUpdate = VSChunkUpdate<std::vector<float>>;

    std::map<VSChunk*, std::shared_ptr<VSShadwoChunkUpdate>> activeShadowBuildTasks;

    using VSVisibilityChunkUpdate = VSChunkUpdate<VSChunk::VSVisibleBlockInfos>;

    std::map<VSChunk*, std::shared_ptr<VSVisibilityChunkUpdate>> activeVisibilityBuildTasks;

    const static inline auto maxShadowUpdateThreads = std::thread::hardware_concurrency();

    const static inline std::vector<float> blockEmission = {
        /*Air=0*/ 0.F,
        /*Stone=1*/ 0.F,
        /*Water=2*/ 0.F,
        /*Grass=3*/ 0.F,
        /*Wood=4*/ 0.F,
        /*Sand=5*/ 0.F,
        /*Leaf=6*/ 0.F,
        /*Lava=7*/ 16.F};

    void initializeChunks();

    glm::ivec2 getChunkCount() const;

    VSChunk* createChunk() const;

    void deleteChunk(VSChunk* chunk);

    void updateShadows(std::size_t chunkIndex);

    std::vector<float> chunkUpdateShadow(
        const std::atomic<bool>& bShouldCancel,
        std::atomic<bool>& bIsReady,
        std::size_t chunkIndex) const;

    void updateVisibleBlocks(std::size_t chunkIndex);

    VSChunk::VSVisibleBlockInfos chunkUpdateVisibility(
        const std::atomic<bool>& bShouldCancel,
        std::atomic<bool>& bIsReady,
        std::size_t chunkIndex) const;

    std::uint8_t isBlockVisible(std::size_t chunkIndex, std::size_t blockIndex) const;

    std::uint8_t
    isCenterBlockVisible(std::size_t chunkIndex, const glm::ivec3& blockCoordinates) const;

    std::uint8_t
    isBorderBlockVisible(std::size_t chunkIndex, const glm::ivec3& blockCoordinates) const;

    bool isAtWorldBorder(const glm::ivec3& blockWorldCoordinates) const;

    std::array<std::uint32_t, 6> getLightInformation(const glm::vec3& blockCoordinates) const;

    std::uint32_t getLightInformationForFace(
        const glm::vec3& blockWorldCoordinates,
        const std::array<glm::vec3, 4>& corners) const;

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