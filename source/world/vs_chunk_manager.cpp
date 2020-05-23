#include "world/vs_chunk_manager.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <array>

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
    for (std::size_t i = 1; i < visibleBlockInfos.size(); i++)
    {
        auto* vertexContext = loadVertexContext("models/cubes/" + std::to_string(i) + ".obj");
        vertexContexts[i] = vertexContext;

        glBindVertexArray(vertexContext->vertexArrayObject);

        auto nextAttribPointer = vertexContext->lastAttribPointer + 1;
        glGenBuffers(1, &visibleBlockInfoBuffers[i]);
        glBindBuffer(GL_ARRAY_BUFFER, visibleBlockInfoBuffers[i]);
        glBufferData(
            GL_ARRAY_BUFFER,
            visibleBlockInfos[i].size() * sizeof(VSChunk::VSVisibleBlockInfo),
            &visibleBlockInfos[i][0],
            GL_STATIC_DRAW);

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
        {0.F, 0.F, 0.F}, {0.3F, 0.3F, 0.3F}, {0.01F, 0.5F, 0.15F}};
    blockColors.reserve(256);

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

void VSChunkManager::draw(VSWorld* world)
{
    for (auto& info : visibleBlockInfos)
    {
        info.clear();
    }

    for (const auto* chunk : chunks)
    {
        if (VSApp::getInstance()->getUI()->getState()->bShouldDrawChunkBorder)
        {
            const auto chunkPos = glm::vec3(chunk->modelMatrix[3]);
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

        const auto chunkCenterInP = VP * glm::vec4(chunk->modelMatrix[3]);

        const auto horizontalRadius =
            glm::sqrt(chunkSize.x * chunkSize.x + chunkSize.z * chunkSize.z);

        if ((chunkCenterInP.z - horizontalRadius) < world->getCamera()->getZFar() * 1.F &&
            (chunkCenterInP.z + horizontalRadius) > world->getCamera()->getZNear() * 1.F &&
            (glm::abs(chunkCenterInP.x) - horizontalRadius) < (chunkCenterInP.w * 1.F) &&
            (glm::abs(chunkCenterInP.y) - chunkSize.y) < (chunkCenterInP.w * 1.F))
        {
            for (std::size_t i = 0; i < chunk->visibleBlockInfos.size(); i++)
            {
                visibleBlockInfos[i].insert(
                    visibleBlockInfos[i].end(),
                    chunk->visibleBlockInfos[i].begin(),
                    chunk->visibleBlockInfos[i].end());
            }
        }
    }

    drawCallCount = 0;

    for (std::size_t i = 1; i < visibleBlockInfos.size(); i++)
    {
        chunkShader.uniforms()
            .setVec3("lightPos", world->getDirectLightPos())
            .setVec3("lightColor", world->getDirectLightColor())
            .setVec3("viewPos", world->getCamera()->getPosition())
            .setMat4("VP", world->getCamera()->getVPMatrix());

        // dont draw if no blocks active
        if (!visibleBlockInfos[i].empty())
        {
            glBindVertexArray(vertexContexts[i]->vertexArrayObject);

            glBindBuffer(GL_ARRAY_BUFFER, visibleBlockInfoBuffers[i]);
            glBufferData(
                GL_ARRAY_BUFFER,
                visibleBlockInfos[i].size() * sizeof(VSChunk::VSVisibleBlockInfo),
                &(visibleBlockInfos[i][0]),
                GL_STATIC_DRAW);

            glDrawElementsInstanced(
                GL_TRIANGLES,
                vertexContexts[i]->indexCount,
                GL_UNSIGNED_INT,
                nullptr,
                visibleBlockInfos[i].size());

            glBindVertexArray(0);

            drawCallCount++;
        }
    }
}

void VSChunkManager::updateChunks()
{
    assert(debug_isMainThread());

    initializeChunks();
    for (std::size_t chunkIndex = 0; chunkIndex < getTotalChunkCount(); ++chunkIndex)
    {
        // to avoid stutter we only update one chunk per frame
        if (updateVisibleBlocks(chunkIndex))
        {
            return;
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
    return std::accumulate(
        visibleBlockInfos.begin(),
        visibleBlockInfos.end(),
        0,
        [](std::size_t acc, const std::vector<VSChunk::VSVisibleBlockInfo>& curr) {
            return acc + curr.size();
        });
}

std::size_t VSChunkManager::getTotalChunkCount() const
{
    return glm::compMul(chunkCount);
}

std::size_t VSChunkManager::getDrawCallCount() const
{
    return drawCallCount;
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
                newChunk->modelMatrix = glm::translate(
                    newChunk->modelMatrix,
                    glm::vec3(
                        chunkSize.x * (static_cast<float>(x) + 0.5F),
                        0.F,
                        chunkSize.z * (static_cast<float>(y) + 0.5F)) -
                        (glm::vec3(chunkSize.x * chunkCount.x, 0.F, chunkSize.z * chunkCount.y) /
                         2.F));
                chunks[y * chunkCount.x + x] = newChunk;
            }
        }
    }
}

VSChunkManager::VSChunk* VSChunkManager::createChunk() const
{
    auto* chunk = new VSChunk();

    chunk->blocks.resize(getChunkBlockCount(), VS_DEFAULT_BLOCK_ID);

    return chunk;
}

void VSChunkManager::deleteChunk(VSChunk* chunk)
{
    delete chunk;
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
        for (std::size_t blockIndex = 0; blockIndex < getChunkBlockCount(); blockIndex++)
        {
            if (chunk->blocks[blockIndex] != VS_DEFAULT_BLOCK_ID)
            {
                const auto blockType = isBlockVisible(chunkIndex, blockIndex);
                if (blockType != 0)
                {
                    const auto offset = chunk->modelMatrix *
                                        (glm::vec4(
                                            glm::vec3(blockIndexToBlockCoordinates(blockIndex)) +
                                                glm::vec3(0.5F) - glm::vec3(chunkSize) / 2.F,
                                            1.F));
                    chunk->visibleBlockInfos[blockType].emplace_back(
                        VSChunk::VSVisibleBlockInfo{offset, chunk->blocks[blockIndex]});
                }
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
    const glm::ivec2 chunkCoordinates = chunkIndexToChunkCoordinates(chunkIndex);
    return {
        chunkCoordinates.x * chunkSize.x + blockCoords.x,
        blockCoords.y,
        chunkCoordinates.y * chunkSize.z + blockCoords.z};
}
