#include "world/vs_chunk_manager.h"

#include "renderer/vs_modelloader.h"
#include "renderer/vs_shader.h"

#include "world/vs_world.h"
#include "core/vs_camera.h"
#include "core/vs_app.h"
#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"

#include "core/vs_debug_draw.h"

VSChunkManager::VSChunkManager() {
}

VSChunkManager::VSChunk* VSChunkManager::createChunk() {
    auto* chunk = new VSChunk();

    chunk->blocks.resize(glm::compMul(chunkSize), VS_DEFAULT_BLOCK_ID);

    chunk->vertexContext = loadVertexContext("models/cube.obj");

    glBindVertexArray(chunk->vertexContext->vertexArrayObject);

    const auto nextAttribPointer = chunk->vertexContext->lastAttribPointer + 1;
    glGenBuffers(1, &chunk->activeBlocksInstanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, chunk->activeBlocksInstanceBuffer);
    glEnableVertexAttribArray(nextAttribPointer);
    glVertexAttribPointer(nextAttribPointer, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBufferData(
        GL_ARRAY_BUFFER,
        chunk->drawnBlocksOffsets.size() * sizeof(glm::vec3),
        &chunk->drawnBlocksOffsets[0],
        GL_STATIC_DRAW);
    glVertexAttribDivisor(nextAttribPointer, 1);

    glBindVertexArray(0);

    return chunk;
}

void VSChunkManager::deleteChunk(VSChunk* chunk) {
    glBindVertexArray(chunk->vertexContext->vertexArrayObject);
    glDeleteBuffers(1, &chunk->activeBlocksInstanceBuffer);
    delete chunk;
}

VSBlockID VSChunkManager::getBlock(glm::ivec3 location) const
{
    const auto [chunkIndex, blockIndex] = worldCoordinatesToChunkAndBlockIndex(location);
    return chunks[chunkIndex]->blocks[blockIndex];
}

void VSChunkManager::setBlock(glm::ivec3 location, VSBlockID blockID)
{
    const auto [chunkIndex, blockIndex] = worldCoordinatesToChunkAndBlockIndex(location);
    chunks[chunkIndex]->blocks[blockIndex] = blockID;
    chunks[chunkIndex]->bIsDirty = true;
}

glm::ivec3 VSChunkManager::getWorldSize() const
{
    return worldSize;
}

void VSChunkManager::draw(VSWorld* world) const
{
    chunkShader.uniforms()
        .setVec3("lightPos", world->getDirectLightPos())
        .setVec3("lightColor", world->getDirectLightColor())
        .setVec3("viewPos", world->getCamera()->getPosition())
        .setVec3("chunkSize", chunkSize);
    
    for (const auto chunk : chunks)
    {
        if (VSApp::getInstance()->getUI()->getState()->bShouldDrawChunkBorder) {
            const auto chunkPos = glm::vec3(chunk->modelMatrix[3]);
            world->getDebugDraw()->drawBox({chunkPos - glm::vec3(chunkSize / 2), chunkPos + glm::vec3(chunkSize / 2)}, {255, 0, 0});
        }
        // dont draw if no blocks active
        if (!chunk->drawnBlocksOffsets.empty())
        {
            chunkShader.uniforms()
                .setMat4("model", chunk->modelMatrix)
                .setMat4("MVP", world->getCamera()->getMVPMatrixFast(chunk->modelMatrix));

            glBindVertexArray(chunk->vertexContext->vertexArrayObject);
            glDrawElementsInstanced(
                GL_TRIANGLES,
                chunk->vertexContext->triangleCount,
                GL_UNSIGNED_INT,
                nullptr,
                chunk->drawnBlocksOffsets.size());
            glBindVertexArray(0);
        }
    }
}

void VSChunkManager::updateChunks()
{
    assert(debug_isMainThread());

    initializeChunks();
    for (int chunkIndex = 0; chunkIndex < glm::compMul(chunkCount); ++chunkIndex)
    {
        // to avoid stutter we only update one chunk per frame
        if (updateVisibleBlocks(chunkIndex)) {
            return;
        }
    }
}
void VSChunkManager::setChunkDimensions(
    const glm::ivec3& inChunkSize,
    const glm::ivec2& inChunkCount)
{
    chunkSize = inChunkSize;
    chunkCount = inChunkCount;
    worldSize = {chunkSize.x * chunkCount.x, chunkSize.y, chunkSize.z * chunkCount.y};
    bShouldReinitializeChunks = true;
}

void VSChunkManager::initializeChunks()
{
    bool expected = true;
    if (bShouldReinitializeChunks.compare_exchange_weak(expected, false))
    {
        for (auto& chunk : chunks)
        {
            deleteChunk(chunk);
        }
        chunks.empty();
        chunks.resize(chunkCount.x * chunkCount.y);

        for (int y = 0; y < chunkCount.x; y++)
        {
            for (int x = 0; x < chunkCount.x; x++)
            {
                auto newChunk = createChunk();
                newChunk->modelMatrix = glm::translate(
                    newChunk->modelMatrix, glm::vec3(chunkSize.x * x, 0.f, chunkSize.z * y));
                chunks[y * chunkCount.x + x] = newChunk;
            }
        }
    }
}

bool VSChunkManager::updateVisibleBlocks(std::size_t chunkIndex)
{
    const auto chunk = chunks[chunkIndex];
    bool expected = true;
    if (chunk->bIsDirty.compare_exchange_weak(expected, false))
    {
        chunk->drawnBlocksOffsets.clear();
        for (std::size_t blockIndex = 0;
             blockIndex < static_cast<std::size_t>(glm::compMul(chunkSize));
             blockIndex++)
        {
            if (chunk->blocks[blockIndex] != VS_DEFAULT_BLOCK_ID &&
                isBlockVisible(chunkIndex, blockIndex))
            {
                const auto offset = glm::vec3(blockIndexToBlockCoordinates(blockIndex)) -
                                    glm::vec3(chunkSize) / 2.f;
                chunk->drawnBlocksOffsets.push_back(offset);
            }
        }

        glBindVertexArray(chunk->vertexContext->vertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, chunk->activeBlocksInstanceBuffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            chunk->drawnBlocksOffsets.size() * sizeof(glm::vec3),
            &chunk->drawnBlocksOffsets[0],
            GL_STATIC_DRAW);

        glBindVertexArray(0);

        return true;
    }
    return false;
}

bool VSChunkManager::isBlockVisible(std::size_t chunkIndex, std::size_t blockIndex) const
{
    const auto blockCoords = blockIndexToBlockCoordinates(blockIndex);

    if (blockCoords.x == 0 || blockCoords.x == chunkSize.x - 1 || blockCoords.y == 0 ||
        blockCoords.y == chunkSize.y - 1 || blockCoords.z == 0 ||
        blockCoords.z == chunkSize.z - 1)
    {
        return isBorderBlockVisible(chunkIndex, blockCoords);
    }
    return isCenterBlockVisible(chunkIndex, blockCoords);
}

bool VSChunkManager::isCenterBlockVisible(
    std::size_t chunkIndex,
    const glm::ivec3& blockCoordinates) const
{
    const auto& blocks = chunks[chunkIndex]->blocks;

    const auto right =
        glm::ivec3(blockCoordinates.x + 1, blockCoordinates.y, blockCoordinates.z);
    const auto left =
        glm::ivec3(blockCoordinates.x - 1, blockCoordinates.y, blockCoordinates.z);

    const auto top = glm::ivec3(blockCoordinates.x, blockCoordinates.y + 1, blockCoordinates.z);
    const auto down =
        glm::ivec3(blockCoordinates.x, blockCoordinates.y - 1, blockCoordinates.z);

    const auto front =
        glm::ivec3(blockCoordinates.x, blockCoordinates.y, blockCoordinates.z + 1);
    // TODO is + 1 or -1 back?
    const auto back =
        glm::ivec3(blockCoordinates.x, blockCoordinates.y, blockCoordinates.z - 1);

    return blocks[blockCoordinatesToBlockIndex(right)] == VS_DEFAULT_BLOCK_ID ||
           blocks[blockCoordinatesToBlockIndex(left)] == VS_DEFAULT_BLOCK_ID ||
           blocks[blockCoordinatesToBlockIndex(top)] == VS_DEFAULT_BLOCK_ID ||
           blocks[blockCoordinatesToBlockIndex(down)] == VS_DEFAULT_BLOCK_ID ||
           blocks[blockCoordinatesToBlockIndex(front)] == VS_DEFAULT_BLOCK_ID ||
           blocks[blockCoordinatesToBlockIndex(back)] == VS_DEFAULT_BLOCK_ID;
}

bool VSChunkManager::isBorderBlockVisible(
    std::size_t chunkIndex,
    const glm::ivec3& blockCoordinates) const
{
    glm::ivec3 blockWorldCoordinates = blockCoordinatesToWorldCoordinates(
        chunkIndex, blockCoordinates);
    if (blockWorldCoordinates.x == 0 || blockWorldCoordinates.x == worldSize.x - 1 || blockWorldCoordinates.y == 0 ||
        blockWorldCoordinates.y == worldSize.y - 1 || blockWorldCoordinates.z == 0 ||
        blockWorldCoordinates.z == worldSize.z - 1)
    {
        const auto top = blockCoordinatesToWorldCoordinates(
            chunkIndex, glm::ivec3(blockCoordinates.x, blockCoordinates.y + 1, blockCoordinates.z));

        if (top.y <= worldSize.y && getBlock(top) == VS_DEFAULT_BLOCK_ID) {
            return true;
        }
        return false;
    }

    const auto right = blockCoordinatesToWorldCoordinates(
        chunkIndex, glm::ivec3(blockCoordinates.x + 1, blockCoordinates.y, blockCoordinates.z));

    const auto left = blockCoordinatesToWorldCoordinates(
        chunkIndex, glm::ivec3(blockCoordinates.x - 1, blockCoordinates.y, blockCoordinates.z));

    const auto top = blockCoordinatesToWorldCoordinates(
        chunkIndex, glm::ivec3(blockCoordinates.x, blockCoordinates.y + 1, blockCoordinates.z));

    const auto down = blockCoordinatesToWorldCoordinates(
        chunkIndex, glm::ivec3(blockCoordinates.x, blockCoordinates.y - 1, blockCoordinates.z));

    const auto front = blockCoordinatesToWorldCoordinates(
        chunkIndex, glm::ivec3(blockCoordinates.x, blockCoordinates.y, blockCoordinates.z + 1));

    // TODO is + 1 or -1 back?
    const auto back = blockCoordinatesToWorldCoordinates(
        chunkIndex, glm::ivec3(blockCoordinates.x, blockCoordinates.y, blockCoordinates.z - 1));

    return getBlock(right) == VS_DEFAULT_BLOCK_ID || getBlock(left) == VS_DEFAULT_BLOCK_ID ||
           getBlock(top) == VS_DEFAULT_BLOCK_ID || getBlock(down) == VS_DEFAULT_BLOCK_ID ||
           getBlock(front) == VS_DEFAULT_BLOCK_ID || getBlock(back) == VS_DEFAULT_BLOCK_ID;
}

std::size_t VSChunkManager::chunkCoordinatesToChunkIndex(glm::ivec2 chunkCoordinates) const
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

    int x = blockIndex % width;
    int z = blockIndex / (width * height);
    int y = (blockIndex / width) % height;
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
