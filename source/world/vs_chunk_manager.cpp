#include "world/vs_chunk_manager.h"

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <array>
#include <glm/gtx/norm.hpp>
#include <vector>
#include <functional>

#include "renderer/vs_modelloader.h"
#include "renderer/vs_textureloader.h"

#include "world/vs_block.h"
#include "world/vs_world.h"

#include "core/vs_camera.h"
#include "core/vs_app.h"
#include "core/vs_debug_draw.h"

#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"

enum VSCubeFace : std::uint8_t
{
    Front = 0,
    Back = 1,
    Bottom = 2,
    Right = 3,
    Top = 4,
    Left = 5
};

VSChunkManager::VSChunkManager()
{
    spriteTextureID = 0;
    shadowTextureID = 1;

    for (std::size_t i = 1; i < 64; i++)
    {
        auto* vertexContext = loadVertexContext("models/cubes/" + std::to_string(i) + ".obj");
        vertexContexts[i] = vertexContext;

        glBindVertexArray(vertexContext->vertexArrayObject);

        auto nextAttribPointer = vertexContext->lastAttribPointer + 1;
        glGenBuffers(1, &visibleBlockInfoBuffers[i]);
        glBindBuffer(GL_ARRAY_BUFFER, visibleBlockInfoBuffers[i]);

        glEnableVertexAttribArray(nextAttribPointer);
        glVertexAttribPointer(
            nextAttribPointer,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(VSChunk::VSVisibleBlockInfo),
            (void*)offsetof(VSChunk::VSVisibleBlockInfo, locationWorldSpace));

        glVertexAttribDivisor(nextAttribPointer, 1);

        nextAttribPointer++;

        glEnableVertexAttribArray(nextAttribPointer);
        glVertexAttribIPointer(
            nextAttribPointer,
            1,
            GL_BYTE,
            sizeof(VSChunk::VSVisibleBlockInfo),
            (void*)offsetof(VSChunk::VSVisibleBlockInfo, id));
        glVertexAttribDivisor(nextAttribPointer, 1);

        glBindVertexArray(0);
    }

    std::vector<glm::vec3> blockColors = {
        /*Air=0*/ {0.F, 0.F, 0.F},
        /*Stone=1*/ {0.3F, 0.3F, 0.3F},
        /*Water=2*/ {0.F, 0.F, 0.5F},
        /*Grass=3*/ {0.F, 0.3F, 0.F},
        /*Wood=4*/ {0.5F, 0.25F, 0.1F},
        /*Sand=5*/ {1.F, 0.9F, 0.5F},
        /*Leaf=6*/ {0.F, 0.5F, 0.F}};
    spriteTexture = TextureAtlasFromFile("textures/tiles");

    chunkShader.uniforms()
        .setVec3Array("blockColors", blockColors)
        .setInt("spriteTexture", spriteTextureID);
}

VSBlockID VSChunkManager::getBlock(const glm::vec3& location) const
{
    const auto zeroBaseLocation = glm::ivec3(glm::floor(location)) + worldSizeHalf;
    const auto [chunkIndex, blockIndex] = worldCoordinatesToChunkAndBlockIndex(zeroBaseLocation);
    return chunks[chunkIndex]->blocks[blockIndex];
}

void VSChunkManager::setBlock(const glm::vec3& location, VSBlockID blockID)
{
    assert(!bShouldReinitializeChunks);
    const auto zeroBaseLocation = glm::ivec3(glm::floor(location)) + worldSizeHalf;
    const auto [chunkCoordinates, blockIndex] =
        worldCoordinatesToChunkCoordinatesAndBlockIndex(zeroBaseLocation);

    const auto chunkIndex = chunkCoordinatesToChunkIndex(chunkCoordinates);

    chunks[chunkIndex]->blocks[blockIndex] = blockID;
    chunks[chunkIndex]->bIsDirty = true;

    const auto right = glm::clamp(chunkCoordinates + glm::ivec2(1, 0), {0, 0}, chunkCount - 1);
    const auto left = glm::clamp(chunkCoordinates - glm::ivec2(1, 0), {0, 0}, chunkCount - 1);

    const auto top = glm::clamp(chunkCoordinates + glm::ivec2(0, 1), {0, 0}, chunkCount - 1);
    const auto down = glm::clamp(chunkCoordinates - glm::ivec2(1, 0), {0, 0}, chunkCount - 1);

    chunks[chunkCoordinatesToChunkIndex(right)]->bIsDirty = true;
    chunks[chunkCoordinatesToChunkIndex(left)]->bIsDirty = true;
    chunks[chunkCoordinatesToChunkIndex(top)]->bIsDirty = true;
    chunks[chunkCoordinatesToChunkIndex(down)]->bIsDirty = true;
}

glm::ivec3 VSChunkManager::getWorldSize() const
{
    return worldSize;
}

glm::ivec2 VSChunkManager::getChunkCount() const
{
    return chunkCount;
}

void VSChunkManager::draw(VSWorld* world)
{
    std::array<std::size_t, faceCombinationCount> visibleBlockInfoCount{};
    std::array<std::size_t, faceCombinationCount> visibleBlockInfoCopiedCount{};
    std::vector<VSChunk*> visibleChunks;
    drawnBlockCount = 0;

    for (auto* chunk : chunks)
    {
        if (VSApp::getInstance()->getUI()->getState()->bShouldDrawChunkBorder)
        {
            const auto chunkPos = chunk->chunkLocation;
            world->getDebugDraw()->drawBox(
                {chunkPos - glm::vec3(chunkSize / 2), chunkPos + glm::vec3(chunkSize / 2)},
                {255, 0, 0});

            world->getDebugDraw()->drawSphere({0, 0, 0}, worldSizeHalf.x, {255, 0, 0});
        }

        glm::mat4 VP = world->getCamera()->getVPMatrix();
        if (VSApp::getInstance()->getUI()->getState()->bShouldFreezeFrustum)
        {
            VP = frozenVPMatrix;
            world->getDebugDraw()->drawFrustum(VP, {0, 255, 0});
        }
        frozenVPMatrix = VP;

        const auto chunkCenterInP = VP * glm::vec4(chunk->chunkLocation, 1.f);

        const auto horizontalRadius =
            glm::sqrt(chunkSize.x * chunkSize.x + chunkSize.z * chunkSize.z);

        if ((chunkCenterInP.z - horizontalRadius) < world->getCamera()->getZFar() * 1.F &&
            (chunkCenterInP.z + horizontalRadius) > world->getCamera()->getZNear() * 1.F &&
            (glm::abs(chunkCenterInP.x) - horizontalRadius) < (chunkCenterInP.w * 1.F) &&
            (glm::abs(chunkCenterInP.y) - chunkSize.y * 2.f) < (chunkCenterInP.w * 1.F))
        {
            for (std::size_t i = 0; i < chunk->visibleBlockInfos.size(); i++)
            {
                visibleBlockInfoCount[i] += chunk->visibleBlockInfos[i].size();
                drawnBlockCount += chunk->visibleBlockInfos[i].size();
            }
            visibleChunks.push_back(chunk);
        }
    }
    glActiveTexture(GL_TEXTURE0 + shadowTextureID);
    glBindTexture(GL_TEXTURE_3D, shadowTexture);

    glActiveTexture(GL_TEXTURE0 + spriteTextureID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, spriteTexture);

    chunkShader.uniforms()
        .setVec3("lightDir", world->getDirectLightDir())
        .setVec3("lightColor", world->getDirectLightColor())
        .setVec3("viewPos", world->getCamera()->getPosition())
        .setMat4("VP", world->getCamera()->getVPMatrix())
        .setUVec3("worldSize", getWorldSize())
        .setInt("shadowTexture", shadowTextureID)
        .setInt("spriteTexture", spriteTextureID)
        .setBool("enableShadows", VSApp::getInstance()->getUI()->getState()->bAreShadowsEnabled)
        .setBool("enableAO", VSApp::getInstance()->getUI()->getState()->bIsAmbientOcclusionEnabled);

    drawCallCount = 0;

    for (std::size_t i = 1; i < faceCombinationCount; i++)
    {
        // dont draw if no blocks active
        if (visibleBlockInfoCount[i] != 0)
        {
            glBindVertexArray(vertexContexts[i]->vertexArrayObject);

            glBindBuffer(GL_ARRAY_BUFFER, visibleBlockInfoBuffers[i]);
            glBufferData(
                GL_ARRAY_BUFFER,
                visibleBlockInfoCount[i] * sizeof(VSChunk::VSVisibleBlockInfo),
                nullptr,
                GL_DYNAMIC_DRAW);

            for (const auto* chunk : visibleChunks)
            {
                glBufferSubData(
                    GL_ARRAY_BUFFER,
                    visibleBlockInfoCopiedCount[i] * sizeof(VSChunk::VSVisibleBlockInfo),
                    chunk->visibleBlockInfos[i].size() * sizeof(VSChunk::VSVisibleBlockInfo),
                    chunk->visibleBlockInfos[i].data());

                visibleBlockInfoCopiedCount[i] += chunk->visibleBlockInfos[i].size();
            }

            glDrawElementsInstanced(
                GL_TRIANGLES,
                vertexContexts[i]->indexCount,
                GL_UNSIGNED_INT,
                nullptr,
                visibleBlockInfoCount[i]);

            drawCallCount++;
        }
    }

    glBindVertexArray(0);
}

void VSChunkManager::updateChunks()
{
    assert(debug_isMainThread());

    initializeChunks();

    // Init from file asynchronous
    bool expected = true;
    if (bShouldInitializeFromData.compare_exchange_weak(expected, false))
    {
        // Set block data to chunks, TODO: add checks, this is potentially dangerous
        uint32_t chunkSize = getChunkBlockCount();
        auto iter = worldDataFromFile.blocks.begin();
        for (const auto chunk : chunks)
        {
            chunk->blocks.insert(chunk->blocks.begin(), iter, iter + chunkSize);
            chunk->bIsDirty = true;
            iter += chunkSize;
        }
    }

    auto blocksUpdated = 0;

    // Todo add conifg option
    constexpr auto udpateLimit = 64 * 128 * 64;

    for (std::size_t chunkIndex = 0; chunkIndex < getTotalChunkCount(); ++chunkIndex)
    {
        // to avoid stutter we only update approx. 64*256*64 blocks per frame
        // this could be moved ot a thread as well, but is fast enough to run on the main thread.
        if (updateVisibleBlocks(chunkIndex))
        {
            blocksUpdated += getChunkBlockCount();
        }

        if (blocksUpdated > udpateLimit)
        {
            return;
        }
    }
    if (VSApp::getInstance()->getUI()->getState()->bAreShadowsEnabled)
    {
        for (std::size_t chunkIndex = 0; chunkIndex < getTotalChunkCount(); ++chunkIndex)
        {
            updateShadows(chunkIndex);
        }
    }
}

void VSChunkManager::setChunkDimensions(
    const glm::ivec3& inChunkSize,
    const glm::ivec2& inChunkCount)
{
    // Force even number of chunks and blocks
    newChunkSize = (inChunkSize / 2) * 2;
    newChunkCount = (inChunkCount / 2) * 2;
    newWorldSize = {
        newChunkSize.x * newChunkCount.x, newChunkSize.y, newChunkSize.z * newChunkCount.y};
    newWorldSizeHalf = newWorldSize / 2;
    bShouldReinitializeChunks = true;
}

std::size_t VSChunkManager::getChunkBlockCount() const
{
    return glm::compMul(chunkSize);
}

std::size_t VSChunkManager::getTotalBlockCount() const
{
    return glm::compMul(chunkSize) * getTotalChunkCount();
}

std::size_t VSChunkManager::getVisibleBlockCount() const
{
    return std::accumulate(chunks.begin(), chunks.end(), 0, [](std::size_t acc, VSChunk* curr) {
        return acc +
               std::accumulate(
                   curr->visibleBlockInfos.begin(),
                   curr->visibleBlockInfos.end(),
                   0,
                   [](std::size_t acc, const std::vector<VSChunk::VSVisibleBlockInfo>& currInner) {
                       return acc + currInner.size();
                   });
    });
}

std::size_t VSChunkManager::getDrawnBlockCount() const
{
    return drawnBlockCount;
}

std::size_t VSChunkManager::getTotalChunkCount() const
{
    return glm::compMul(chunkCount);
}

std::size_t VSChunkManager::getDrawCallCount() const
{
    return drawCallCount;
}

bool VSChunkManager::shouldReinitializeChunks() const
{
    return bShouldReinitializeChunks.load();
}

bool VSChunkManager::isLocationInBounds(const glm::ivec3& location) const
{
    return (
        (location.x >= -worldSizeHalf.x && location.x < worldSizeHalf.x) &&
        (location.y >= -worldSizeHalf.y && location.y < worldSizeHalf.y) &&
        (location.z >= -worldSizeHalf.z && location.z < worldSizeHalf.z));
}

VSChunkManager::VSTraceResult
VSChunkManager::lineTrace(const glm::vec3& start, const glm::vec3& end) const
{
    const glm::vec3 startToEnd = end - start;

    const float maxRayLength = glm::length(startToEnd);

    const auto rayDir = startToEnd / maxRayLength;

    float t = 0.F;

    while (t < maxRayLength)
    {
        const auto samplePos = start + rayDir * t;
        if (isLocationInBounds(samplePos))
        {
            const auto blockSample = getBlock(samplePos);
            if (blockSample != VS_DEFAULT_BLOCK_ID)
            {
                const auto centerToHitPos = glm::fract(samplePos) - 0.5F;

                glm::vec3 d = glm::abs(centerToHitPos) - 0.5F;
                const auto distToSurface = length(glm::max(d, glm::vec3(0.F))) +
                                           glm::min(glm::max(glm::max(d.x, d.y), d.z), 0.F);

                auto interSectionPos = samplePos - rayDir * distToSurface;

                // calc normals

                auto normal = glm::vec3(0, 0, 0);

                if (d.x > d.y && d.x > d.z)
                {
                    normal = {glm::sign(centerToHitPos.x) * 1.F, 0.F, 0.F};
                    interSectionPos.x = glm::round(interSectionPos.x);
                    if (centerToHitPos.x < 0)
                    {
                        interSectionPos.x -= 0.00001F;
                    }
                }

                if (d.y > d.x && d.y > d.z)
                {
                    normal = {0.F, glm::sign(centerToHitPos.y) * 1.F, 0.F};
                    interSectionPos.y = glm::round(interSectionPos.y);
                    if (centerToHitPos.y < 0)
                    {
                        interSectionPos.y -= 0.00001F;
                    }
                }

                if (d.z > d.x && d.z > d.y)
                {
                    normal = {0.F, 0.F, glm::sign(centerToHitPos.z) * 1.F};
                    interSectionPos.z = glm::round(interSectionPos.z);
                    if (centerToHitPos.z < 0)
                    {
                        interSectionPos.z -= 0.00001F;
                    }
                }

                return {true, interSectionPos, normal, blockSample};
            }
        }
        t += 0.005F;
    }

    return {false, {}, {}, VS_DEFAULT_BLOCK_ID};
}

VSChunkManager::VSWorldData VSChunkManager::getData() const
{
    VSChunkManager::VSWorldData worldData{};

    worldData.chunkSize = getWorldSize();
    worldData.chunkCount = getChunkCount();

    // Write BlockIDs to vector
    worldData.blocks.reserve(getChunkBlockCount() * getTotalChunkCount());
    for (const auto chunk : chunks)
    {
        worldData.blocks.insert(worldData.blocks.end(), chunk->blocks.begin(), chunk->blocks.end());
    }

    return worldData;
}

void VSChunkManager::initFromData(const VSWorldData& data)
{
    setChunkDimensions(data.chunkSize, data.chunkCount);
    worldDataFromFile = data;
    bShouldInitializeFromData = true;
}

void VSChunkManager::initializeChunks()
{
    bool expected = true;
    if (bShouldReinitializeChunks.load() == expected)
    {
        chunkSize = newChunkSize;
        chunkCount = newChunkCount;
        worldSize = newWorldSize;
        worldSizeHalf = newWorldSizeHalf;

        for (const auto& [chunk, shadowBuildUpdate] : activeShadowBuildTasks)
        {
            shadowBuildUpdate->cancel();
        }
        activeShadowBuildTasks.clear();
        for (auto* chunk : chunks)
        {
            deleteChunk(chunk);
        }
        chunks.clear();
        chunks.resize(chunkCount.x * chunkCount.y);

        for (int y = 0; y < chunkCount.x; y++)
        {
            for (int x = 0; x < chunkCount.x; x++)
            {
                VSChunk* newChunk = createChunk();
                newChunk->chunkLocation =
                    glm::vec3(
                        chunkSize.x * (static_cast<float>(x) + 0.5F),
                        0.F,
                        chunkSize.z * (static_cast<float>(y) + 0.5F)) -
                    glm::vec3(chunkSize.x * chunkCount.x, 0.F, chunkSize.z * chunkCount.y) / 2.F;
                chunks[y * chunkCount.x + x] = newChunk;
            }
        }

        glDeleteTextures(1, &shadowTexture);

        glGenTextures(1, &shadowTexture);
        glBindTexture(GL_TEXTURE_3D, shadowTexture);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
        glm::vec3 borderColor(std::numeric_limits<float>::max());
        glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, &borderColor[0]);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage3D(
            GL_TEXTURE_3D,
            0,
            GL_R16F,
            worldSize.x,
            worldSize.y,
            worldSize.z,
            0,
            GL_RED,
            GL_FLOAT,
            nullptr);

        bShouldReinitializeChunks.compare_exchange_weak(expected, false);
    }
}

VSChunkManager::VSChunk* VSChunkManager::createChunk() const
{
    auto* chunk = new VSChunk();

    chunk->blocks.resize(getChunkBlockCount(), VS_DEFAULT_BLOCK_ID);
    chunk->bIsBlockVisible.resize(getChunkBlockCount(), false);

    return chunk;
}

void VSChunkManager::deleteChunk(VSChunk* chunk)
{
    delete chunk;
}

void VSChunkManager::updateShadows(std::size_t chunkIndex)
{
    auto* const chunk = chunks[chunkIndex];
    bool expectedShadows = true;
    // check if dirty after checking for shadows to avoid race conditions
    // only allow hardware_concurrency active chunk updates
    if (activeShadowBuildTasks.size() < maxShadowUpdateThreads &&
        chunk->bShouldRebuildShadows.compare_exchange_weak(expectedShadows, false))
    {
        if (activeShadowBuildTasks.count(chunk) != 0)
        {
            activeShadowBuildTasks[chunk]->cancel();
        }

        const auto shadowUpdate = VSShadwoChunkUpdate::create(
            [this](
                const std::atomic<bool>& bShouldCancel,
                std::atomic<bool>& bIsReady,
                std::size_t chunkIndex) {
                return this->chunkUpdateShadow(bShouldCancel, bIsReady, chunkIndex);
            },
            chunkIndex);

        activeShadowBuildTasks.emplace(chunk, shadowUpdate);
    }

    if (activeShadowBuildTasks.count(chunk) != 0)
    {
        const auto shadowTask = activeShadowBuildTasks[chunk];
        if (shadowTask->isReady())
        {
            const auto chunkDistanceField = shadowTask->getResult();
            activeShadowBuildTasks.erase(chunk);

            const auto textureBlockLocation = chunk->chunkLocation +
                                              (glm::vec3(worldSize.x, 0.F, worldSize.z) / 2.F) -
                                              (glm::vec3(chunkSize.x, 0.F, chunkSize.z) / 2.F);

            glBindTexture(GL_TEXTURE_3D, shadowTexture);

            glTexSubImage3D(
                GL_TEXTURE_3D,
                0,
                textureBlockLocation.x,
                textureBlockLocation.y,
                textureBlockLocation.z,
                chunkSize.x,
                chunkSize.y,
                chunkSize.z,
                GL_RED,
                GL_FLOAT,
                chunkDistanceField.data());
        }
    }
}

std::vector<float> VSChunkManager::chunkUpdateShadow(
    const std::atomic<bool>& bShouldCancel,
    std::atomic<bool>& bIsReady,
    std::size_t chunkIndex) const
{
    const auto chunkCoords = chunkIndexToChunkCoordinates(chunkIndex);

    std::vector<VSChunk::VSVisibleBlockInfo> relevantVisibleBlocks;

    // TODO this wont work anymore if the terrain becomes more complex
    // overhangs or floating stuff will cause issues
    const std::int32_t chunkRadius =
        1;  // glm::min(1, 128 / static_cast<int>(glm::sqrt(chunkSize.x * chunkSize.x +
    // chunkSize.z * chunkSize.z)));

    for (int x = glm::max(chunkCoords.x - chunkRadius, 0);
         x <= glm::min(chunkCoords.x + chunkRadius, chunkCount.x - 1);
         x++)
    {
        for (int y = glm::max(chunkCoords.y - chunkRadius, 0);
             y <= glm::min(chunkCoords.y + chunkRadius, chunkCount.y - 1);
             y++)
        {
            // abort calculations if canceled
            if (bShouldCancel)
            {
                return {};
            }
            const auto* neighbourChunk = chunks[chunkCoordinatesToChunkIndex({x, y})];
            for (const auto& visibleBlockInfos : neighbourChunk->visibleBlockInfos)
            {
                relevantVisibleBlocks.insert(
                    relevantVisibleBlocks.end(),
                    visibleBlockInfos.begin(),
                    visibleBlockInfos.end());
            }
        }
    }

    auto* const chunk = chunks[chunkIndex];

    std::vector<float> chunkDistanceField;
    chunkDistanceField.resize(getChunkBlockCount());

    const auto chunkToWorld = chunk->chunkLocation + glm::vec3(0.5F) - glm::vec3(chunkSize) / 2.F;

    for (std::size_t blockIndex = 0; blockIndex < getChunkBlockCount(); blockIndex++)
    {
        // abort calculations if canceled
        if (bShouldCancel)
        {
            return {};
        }
        glm::vec3 samplePos = chunkToWorld + glm::vec3(blockIndexToBlockCoordinates(blockIndex));

        float distance = std::numeric_limits<float>::max();

        if (chunk->blocks[blockIndex] != VS_DEFAULT_BLOCK_ID)
        {
            if (chunk->bIsBlockVisible[blockIndex])
            {
                distance = 0.F;
            }
            else
            {
                distance = -0.5F;
            }
        }
        else
        {
            for (const auto& blockCandidate : relevantVisibleBlocks)
            {
                distance =
                    glm::min(distance, glm::length2(samplePos - blockCandidate.locationWorldSpace));
            }
            distance = glm::sqrt(distance);
        }

        chunkDistanceField[blockIndex] = distance;
    }

    bIsReady = true;

    return chunkDistanceField;
}

bool VSChunkManager::updateVisibleBlocks(std::size_t chunkIndex)
{
    auto* const chunk = chunks[chunkIndex];
    bool expected = true;
    if (chunk->bIsDirty.compare_exchange_weak(expected, false))
    {
        for (auto& info : chunk->visibleBlockInfos)
        {
            info.clear();
        }

        const auto chunkBlockCount = getChunkBlockCount();

#pragma omp parallel for
        for (int blockIndex = 0; blockIndex < static_cast<int>(chunkBlockCount); blockIndex++)
        {
            if (chunk->blocks[blockIndex] != VS_DEFAULT_BLOCK_ID)
            {
                const auto blockType = isBlockVisible(chunkIndex, blockIndex);
                if (blockType != 0)
                {
                    const auto offset = chunk->chunkLocation +
                                        glm::vec3(blockIndexToBlockCoordinates(blockIndex)) +
                                        glm::vec3(0.5F) - glm::vec3(chunkSize) / 2.F;

                    const auto blockInfo =
                        VSChunk::VSVisibleBlockInfo{offset, chunk->blocks[blockIndex]};

#pragma omp critical
                    chunk->visibleBlockInfos[blockType].emplace_back(blockInfo);
                    chunk->bIsBlockVisible[blockIndex] = true;
                }
                else
                {
                    chunk->bIsBlockVisible[blockIndex] = false;
                }
            }
            else
            {
                chunk->bIsBlockVisible[blockIndex] = false;
            }
        }

        // update shadows for us and neighbours
        // TODO duplicate code (see updateShadows)
        const auto chunkCoords = chunkIndexToChunkCoordinates(chunkIndex);
        const std::int32_t chunkRadius = 1;
        for (int x = glm::max(chunkCoords.x - chunkRadius, 0);
             x <= glm::min(chunkCoords.x + chunkRadius, chunkCount.x - 1);
             x++)
        {
            for (int y = glm::max(chunkCoords.y - chunkRadius, 0);
                 y <= glm::min(chunkCoords.y + chunkRadius, chunkCount.y - 1);
                 y++)
            {
                auto* neighbourChunk = chunks[chunkCoordinatesToChunkIndex({x, y})];
                neighbourChunk->bShouldRebuildShadows = true;
            }
        }

        return true;
    }
    return false;
}

std::uint8_t VSChunkManager::isBlockVisible(std::size_t chunkIndex, std::size_t blockIndex) const
{
    const auto blockCoords = blockIndexToBlockCoordinates(blockIndex);

    if (blockCoords.x == 0 || blockCoords.x == chunkSize.x - 1 || blockCoords.y == 0 ||
        blockCoords.y == chunkSize.y - 1 || blockCoords.z == 0 || blockCoords.z == chunkSize.z - 1)
    {
        return isBorderBlockVisible(chunkIndex, blockCoords);
    }
    return isCenterBlockVisible(chunkIndex, blockCoords);
}

std::uint8_t VSChunkManager::isCenterBlockVisible(
    std::size_t chunkIndex,
    const glm::ivec3& blockCoordinates) const
{
    const auto& blocks = chunks[chunkIndex]->blocks;

    const auto right = glm::ivec3(blockCoordinates.x + 1, blockCoordinates.y, blockCoordinates.z);
    const auto left = glm::ivec3(blockCoordinates.x - 1, blockCoordinates.y, blockCoordinates.z);

    const auto top = glm::ivec3(blockCoordinates.x, blockCoordinates.y + 1, blockCoordinates.z);
    const auto bottom = glm::ivec3(blockCoordinates.x, blockCoordinates.y - 1, blockCoordinates.z);

    const auto front = glm::ivec3(blockCoordinates.x, blockCoordinates.y, blockCoordinates.z + 1);
    // TODO is + 1 or -1 back?
    const auto back = glm::ivec3(blockCoordinates.x, blockCoordinates.y, blockCoordinates.z - 1);

    std::uint8_t encoded = 0;
    encoded |= static_cast<int>(blocks[blockCoordinatesToBlockIndex(right)] == VS_DEFAULT_BLOCK_ID)
               << VSCubeFace::Right;
    encoded |= static_cast<int>(blocks[blockCoordinatesToBlockIndex(left)] == VS_DEFAULT_BLOCK_ID)
               << VSCubeFace::Left;
    encoded |= static_cast<int>(blocks[blockCoordinatesToBlockIndex(top)] == VS_DEFAULT_BLOCK_ID)
               << VSCubeFace::Top;
    encoded |= static_cast<int>(blocks[blockCoordinatesToBlockIndex(bottom)] == VS_DEFAULT_BLOCK_ID)
               << VSCubeFace::Bottom;
    encoded |= static_cast<int>(blocks[blockCoordinatesToBlockIndex(front)] == VS_DEFAULT_BLOCK_ID)
               << VSCubeFace::Front;
    encoded |= static_cast<int>(blocks[blockCoordinatesToBlockIndex(back)] == VS_DEFAULT_BLOCK_ID)
               << VSCubeFace::Back;

    return encoded;
}

std::uint8_t VSChunkManager::isBorderBlockVisible(
    std::size_t chunkIndex,
    const glm::ivec3& blockCoordinates) const
{
    const auto blockWorldCoordinates =
        blockCoordinatesToWorldCoordinates(chunkIndex, blockCoordinates);
    if (blockWorldCoordinates.x == -worldSizeHalf.x ||
        blockWorldCoordinates.x == worldSizeHalf.x - 1 ||
        blockWorldCoordinates.y == -worldSizeHalf.y ||
        blockWorldCoordinates.y == worldSizeHalf.y - 1 ||
        blockWorldCoordinates.z == -worldSizeHalf.z ||
        blockWorldCoordinates.z == worldSizeHalf.z - 1)
    {
        if (blockWorldCoordinates.y + 1 < worldSizeHalf.y &&
            getBlock(blockWorldCoordinates + glm::ivec3(0, 1, 0)) == VS_DEFAULT_BLOCK_ID)
        {
            // always use a full block at the world border for now
            return 63;
        }
        else
        {
            return 0;
        }
    }

    const auto right = blockWorldCoordinates + glm::ivec3(1, 0, 0);

    const auto left = blockWorldCoordinates + glm::ivec3(-1, 0, 0);

    const auto top = blockWorldCoordinates + glm::ivec3(0, 1, 0);

    const auto bottom = blockWorldCoordinates + glm::ivec3(0, -1, 0);

    const auto front = blockWorldCoordinates + glm::ivec3(0, 0, 1);

    // TODO is + 1 or -1 back?
    const auto back = blockWorldCoordinates + glm::ivec3(0, 0, -1);

    std::uint8_t encoded = 0;
    encoded |= static_cast<int>(getBlock(right) == VS_DEFAULT_BLOCK_ID) << VSCubeFace::Right;
    encoded |= static_cast<int>(getBlock(left) == VS_DEFAULT_BLOCK_ID) << VSCubeFace::Left;
    encoded |= static_cast<int>(getBlock(top) == VS_DEFAULT_BLOCK_ID) << VSCubeFace::Top;
    encoded |= static_cast<int>(getBlock(bottom) == VS_DEFAULT_BLOCK_ID) << VSCubeFace::Bottom;
    encoded |= static_cast<int>(getBlock(front) == VS_DEFAULT_BLOCK_ID) << VSCubeFace::Front;
    encoded |= static_cast<int>(getBlock(back) == VS_DEFAULT_BLOCK_ID) << VSCubeFace::Back;

    return encoded;
}

std::size_t VSChunkManager::chunkCoordinatesToChunkIndex(const glm::ivec2& chunkCoordinates) const
{
    return chunkCoordinates.y * chunkCount.x + chunkCoordinates.x;
}

glm::ivec2 VSChunkManager::chunkIndexToChunkCoordinates(std::size_t chunkIndex) const
{
    return {chunkIndex % chunkCount.y, chunkIndex / chunkCount.y};
}

std::size_t VSChunkManager::blockCoordinatesToBlockIndex(const glm::ivec3& bloockCoords) const
{
    const int width = chunkSize.x;
    const int height = chunkSize.y;

    return bloockCoords.x + bloockCoords.y * width + bloockCoords.z * width * height;
}

glm::ivec3 VSChunkManager::blockIndexToBlockCoordinates(std::size_t blockIndex) const
{
    const int width = chunkSize.x;
    const int height = chunkSize.y;

    std::size_t x = blockIndex % width;
    std::size_t z = blockIndex / (width * height);
    std::size_t y = (blockIndex / width) % height;
    return {x, y, z};
}

glm::ivec2 VSChunkManager::worldCoordinatesToChunkCoordinates(const glm::ivec3& worldCoords) const
{
    return {worldCoords.x / chunkSize.x, worldCoords.z / chunkSize.z};
}

std::tuple<std::size_t, std::size_t>
VSChunkManager::worldCoordinatesToChunkAndBlockIndex(const glm::ivec3& worldCoords) const
{
    const auto chunkCoordinates = worldCoordinatesToChunkCoordinates(worldCoords);
    const auto chunkIndex = chunkCoordinatesToChunkIndex(chunkCoordinates);
    return {
        chunkIndex,
        blockCoordinatesToBlockIndex(
            {worldCoords.x - chunkCoordinates.x * chunkSize.x,
             worldCoords.y,
             worldCoords.z - chunkCoordinates.y * chunkSize.z})};
}

std::tuple<glm::ivec2, std::size_t>
VSChunkManager::worldCoordinatesToChunkCoordinatesAndBlockIndex(const glm::ivec3& worldCoords) const
{
    const auto chunkCoordinates = worldCoordinatesToChunkCoordinates(worldCoords);
    return {
        chunkCoordinates,
        blockCoordinatesToBlockIndex(
            {worldCoords.x - chunkCoordinates.x * chunkSize.x,
             worldCoords.y,
             worldCoords.z - chunkCoordinates.y * chunkSize.z})};
}

glm::ivec3 VSChunkManager::blockCoordinatesToWorldCoordinates(
    std::size_t chunkIndex,
    const glm::ivec3& blockCoords) const
{
    return chunks[chunkIndex]->chunkLocation + glm::vec3(blockCoords - chunkSize / 2);
}

void VSChunkManager::setWorldData(const VSWorldData& worldData)
{
    worldDataFromFile = worldData;
    bShouldInitializeFromData = true;
}
