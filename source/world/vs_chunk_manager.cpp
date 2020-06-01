#include "world/vs_chunk_manager.h"

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <array>
#include <glm/gtx/norm.hpp>
#include <vector>

#include "renderer/vs_modelloader.h"

#include "world/vs_block.h"
#include "world/vs_world.h"
#include "core/vs_camera.h"
#include "core/vs_app.h"
#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"

#include "core/vs_debug_draw.h"

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

    std::vector<glm::vec3> blockColors = {/*Air=0*/ {0.F, 0.F, 0.F},
                                          /*Stone=1*/ {0.3F, 0.3F, 0.3F},
                                          /*Water=2*/ {0.F, 0.F, 0.5F},
                                          /*Grass=3*/ {0.F, 0.3F, 0.F},
                                          /*Wood=4*/ {0.5F, 0.25F, 0.1F},
                                          /*Sand=5*/ {1.F, 0.9F, 0.5F},
                                          /*Leaf=6*/ {0.F, 0.5F, 0.F}};

    chunkShader.uniforms().setVec3Array("blockColors", blockColors);
}

VSBlockID VSChunkManager::getBlock(glm::ivec3 location) const
{
    const auto [chunkIndex, blockIndex] = worldCoordinatesToChunkAndBlockIndex(location);
    return chunks[chunkIndex]->blocks[blockIndex];
}

void VSChunkManager::setBlock(glm::ivec3 location, VSBlockID blockID)
{
    assert(!bShouldReinitializeChunks);

    const auto [chunkCoordinates, blockIndex] =
        worldCoordinatesToChunkCoordinatesAndBlockIndex(location);

    chunks[chunkCoordinatesToChunkIndex(chunkCoordinates)]->blocks[blockIndex] = blockID;
    chunks[chunkCoordinatesToChunkIndex(chunkCoordinates)]->bIsDirty = true;

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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, shadowTexture);

    chunkShader.uniforms()
        .setVec3("lightPos", world->getDirectLightPos())
        .setVec3("lightColor", world->getDirectLightColor())
        .setVec3("viewPos", world->getCamera()->getPosition())
        .setMat4("VP", world->getCamera()->getVPMatrix())
        .setUVec3("worldSize", getWorldSize())
        .setInt("shadowTexture", 0)
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

    for (std::size_t chunkIndex = 0; chunkIndex < getTotalChunkCount(); ++chunkIndex)
    {
        // to avoid stutter we only update one chunk per frame
        if (updateVisibleBlocks(chunkIndex))
        {
            return;
        }
    }
    if (VSApp::getInstance()->getUI()->getState()->bAreShadowsEnabled)
    {
        for (std::size_t chunkIndex = 0; chunkIndex < getTotalChunkCount(); ++chunkIndex)
        {
            // if we have no more chunk updates
            // start updating shadwos
            if (updateShadows(chunkIndex))
            {
                return;
            }
        }
    }
}

void VSChunkManager::setChunkDimensions(
    const glm::ivec3& inChunkSize,
    const glm::ivec2& inChunkCount)
{
    // Force even number of chunks and blocks
    chunkSize = (inChunkSize / 2) * 2;
    chunkCount = (inChunkCount / 2) * 2;
    worldSize = {chunkSize.x * chunkCount.x, chunkSize.y, chunkSize.z * chunkCount.y};
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
    if (bShouldReinitializeChunks.compare_exchange_weak(expected, false))
    {
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

bool VSChunkManager::updateShadows(std::size_t chunkIndex)
{
    auto* const chunk = chunks[chunkIndex];
    bool expectedShadows = true;
    bool expectedDirty = false;
    // check if dirty after checking for shadows to avoid race conditions
    if (chunk->bShouldRebuildShadows.compare_exchange_weak(expectedShadows, false) &&
        chunk->bIsDirty.compare_exchange_weak(expectedDirty, false))
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

        const auto chunkBlockCount = getChunkBlockCount();

        std::vector<float> chunkDistanceField;
        chunkDistanceField.resize(chunkBlockCount);

#pragma omp parallel for
        for (int blockIndex = 0; blockIndex < static_cast<int>(chunkBlockCount); blockIndex++)
        {
            glm::ivec3 blockCordinates = blockIndexToBlockCoordinates(blockIndex);

            glm::vec blockLocationWorldSpace = chunk->chunkLocation + glm::vec3(blockCordinates) +
                                               glm::vec3(0.5F) - glm::vec3(chunkSize) / 2.F;

            float distance = std::numeric_limits<float>::max();
            if (chunk->blocks[blockIndex] != VS_DEFAULT_BLOCK_ID)
            {
                distance = 0.F;
            }
            else
            {
                auto distanceMaxComponent = std::numeric_limits<float>::max();

                for (const auto& blockCandidate : relevantVisibleBlocks)
                {
                    // https://iquilezles.org/www/articles/distfunctions/distfunctions.htm
                    constexpr auto bounds = glm::vec3(0.5f);
                    const auto direction =
                        abs(blockLocationWorldSpace - blockCandidate.locationWorldSpace) - bounds;
                    auto candidateMaxComponent =
                        glm::min(glm::max(direction.x, glm::max(direction.y, direction.z)), 0.F);
                    const auto directionClamped = glm::max(direction, 0.F);
                    // dot product is equal to lengthSquared
                    const auto candidateDistance =
                        candidateMaxComponent + glm::dot(directionClamped, directionClamped);

                    if (candidateDistance < distance)
                    {
                        distance = candidateDistance;
                        distanceMaxComponent = candidateMaxComponent;
                    }
                }

                distance = glm::sqrt(distance - distanceMaxComponent) + distanceMaxComponent;
            }

            chunkDistanceField[blockIndex] = distance;
        }

        // Diagonal filtering
        std::vector<float> chunkDistanceFieldFiltered;
        chunkDistanceFieldFiltered.resize(chunkBlockCount);

#pragma omp parallel for
        for (int x = 0; x < chunkSize.x; x++)
        {
            for (int y = 0; y < chunkSize.y; y++)
            {
                for (int z = 0; z < chunkSize.z; z++)
                {
                    const auto CenterIndex = blockCoordinatesToBlockIndex({x, y, z});

                    const auto minusOne = glm::clamp(
                        glm::ivec3(x - 1, y - 1, z - 1), glm::ivec3(0), glm::ivec3(chunkSize - 1));
                    const auto plusOne = glm::clamp(
                        glm::ivec3(x + 1, y + 1, z + 1), glm::ivec3(0), glm::ivec3(chunkSize - 1));

                    const auto diag0 =
                        blockCoordinatesToBlockIndex({plusOne.x, plusOne.y, plusOne.z});
                    const auto diag1 =
                        blockCoordinatesToBlockIndex({minusOne.x, plusOne.y, plusOne.z});
                    const auto diag2 =
                        blockCoordinatesToBlockIndex({plusOne.x, minusOne.y, plusOne.z});
                    const auto diag3 =
                        blockCoordinatesToBlockIndex({minusOne.x, plusOne.y, plusOne.z});

                    const auto diag4 =
                        blockCoordinatesToBlockIndex({plusOne.x, plusOne.y, minusOne.z});
                    const auto diag5 =
                        blockCoordinatesToBlockIndex({minusOne.x, plusOne.y, minusOne.z});
                    const auto diag6 =
                        blockCoordinatesToBlockIndex({plusOne.x, minusOne.y, minusOne.z});
                    const auto diag7 =
                        blockCoordinatesToBlockIndex({minusOne.x, minusOne.y, minusOne.z});

                    constexpr auto weight = 1.F / 9.F;

                    const auto filteredDistance =
                        chunkDistanceField[CenterIndex] * weight +
                        chunkDistanceField[diag0] * weight + chunkDistanceField[diag1] * weight +
                        chunkDistanceField[diag2] * weight + chunkDistanceField[diag3] * weight +
                        chunkDistanceField[diag4] * weight + chunkDistanceField[diag5] * weight +
                        chunkDistanceField[diag6] * weight + chunkDistanceField[diag7] * weight;

                    chunkDistanceFieldFiltered[CenterIndex] = filteredDistance;
                }
            }
        }

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
            chunkDistanceFieldFiltered.data());
        return true;
    }
    return false;
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
        chunk->bShouldRebuildShadows = true;
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
    glm::ivec3 blockWorldCoordinates =
        blockCoordinatesToWorldCoordinates(chunkIndex, blockCoordinates);
    if (blockWorldCoordinates.x == 0 || blockWorldCoordinates.x == worldSize.x - 1 ||
        blockWorldCoordinates.y == 0 || blockWorldCoordinates.y == worldSize.y - 1 ||
        blockWorldCoordinates.z == 0 || blockWorldCoordinates.z == worldSize.z - 1)
    {
        const auto top = blockCoordinatesToWorldCoordinates(
            chunkIndex, glm::ivec3(blockCoordinates.x, blockCoordinates.y + 1, blockCoordinates.z));

        if (top.y <= worldSize.y && getBlock(top) == VS_DEFAULT_BLOCK_ID)
        {
            // always use a full block at the world border for now
            return 63;
        }
        else
        {
            return 0;
        }
    }

    const auto right = blockCoordinatesToWorldCoordinates(
        chunkIndex, glm::ivec3(blockCoordinates.x + 1, blockCoordinates.y, blockCoordinates.z));

    const auto left = blockCoordinatesToWorldCoordinates(
        chunkIndex, glm::ivec3(blockCoordinates.x - 1, blockCoordinates.y, blockCoordinates.z));

    const auto top = blockCoordinatesToWorldCoordinates(
        chunkIndex, glm::ivec3(blockCoordinates.x, blockCoordinates.y + 1, blockCoordinates.z));

    const auto bottom = blockCoordinatesToWorldCoordinates(
        chunkIndex, glm::ivec3(blockCoordinates.x, blockCoordinates.y - 1, blockCoordinates.z));

    const auto front = blockCoordinatesToWorldCoordinates(
        chunkIndex, glm::ivec3(blockCoordinates.x, blockCoordinates.y, blockCoordinates.z + 1));

    // TODO is + 1 or -1 back?
    const auto back = blockCoordinatesToWorldCoordinates(
        chunkIndex, glm::ivec3(blockCoordinates.x, blockCoordinates.y, blockCoordinates.z - 1));

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

std::size_t VSChunkManager::blockCoordinatesToBlockIndex(const glm::ivec3& chunkCoords) const
{
    const int width = chunkSize.x;
    const int height = chunkSize.y;

    return chunkCoords.x + chunkCoords.y * width + chunkCoords.z * width * height;
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
    return {chunkIndex,
            blockCoordinatesToBlockIndex({worldCoords.x - chunkCoordinates.x * chunkSize.x,
                                          worldCoords.y,
                                          worldCoords.z - chunkCoordinates.y * chunkSize.z})};
}

std::tuple<glm::ivec2, std::size_t>
VSChunkManager::worldCoordinatesToChunkCoordinatesAndBlockIndex(const glm::ivec3& worldCoords) const
{
    const auto chunkCoordinates = worldCoordinatesToChunkCoordinates(worldCoords);
    return {chunkCoordinates,
            blockCoordinatesToBlockIndex({worldCoords.x - chunkCoordinates.x * chunkSize.x,
                                          worldCoords.y,
                                          worldCoords.z - chunkCoordinates.y * chunkSize.z})};
}

glm::ivec3 VSChunkManager::blockCoordinatesToWorldCoordinates(
    std::size_t chunkIndex,
    const glm::ivec3& blockCoords) const
{
    const glm::ivec2 chunkCoordinates = chunkIndexToChunkCoordinates(chunkIndex);
    return {chunkCoordinates.x * chunkSize.x + blockCoords.x,
            blockCoords.y,
            chunkCoordinates.y * chunkSize.z + blockCoords.z};
}

void VSChunkManager::setWorldData(const VSWorldData& worldData)
{
    worldDataFromFile = worldData;
    bShouldInitializeFromData = true;
}
