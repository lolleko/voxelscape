#include "world/vs_chunk.h"

#include "renderer/vs_modelloader.h"
#include "world/vs_world.h"
#include "core/vs_camera.h"

VSChunk::VSChunk(const glm::ivec3& size, VSBlockID defaultID)
    : size(size)
    , defaultID(defaultID)
{
    blocks.resize(getTotalBlockCount());
    clearBlockData();

    vertexContext = loadVertexContext("models/cube.obj");

    glBindVertexArray(vertexContext->vertexArrayObject);

    const auto nextAttribPointer = vertexContext->lastAttribPointer + 1;
    glGenBuffers(1, &activeBlocksInstanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, activeBlocksInstanceBuffer);
    glEnableVertexAttribArray(nextAttribPointer);
    glVertexAttribPointer(nextAttribPointer, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBufferData(
        GL_ARRAY_BUFFER,
        drawnBlocksOffsets.size() * sizeof(glm::vec3),
        &drawnBlocksOffsets[0],
        GL_STATIC_DRAW);
    glVertexAttribDivisor(nextAttribPointer, 1);

    glBindVertexArray(0);
}

VSChunk::~VSChunk()
{
    glBindVertexArray(vertexContext->vertexArrayObject);
    glDeleteBuffers(1, &activeBlocksInstanceBuffer);
}

void VSChunk::updateActiveBlocks()
{
    drawnBlocksOffsets.clear();
    for (size_t blockIndex = 0; blockIndex < getTotalBlockCount(); blockIndex++)
    {
        // Force update
        updateBlock(blockIndex);
    }

    glBindVertexArray(vertexContext->vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, activeBlocksInstanceBuffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        drawnBlocksOffsets.size() * sizeof(glm::vec3),
        &drawnBlocksOffsets[0],
        GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void VSChunk::clearBlockData()
{
    for (size_t blockIndex = 0; blockIndex < getTotalBlockCount(); blockIndex++)
    {
        blocks[blockIndex] = defaultID;
    }
}

void VSChunk::setBlock(glm::ivec3 location, VSBlockID blockID)
{
    blocks[blockCoordsToBlockIndex(location)] = blockID;
}

int VSChunk::getTotalBlockCount() const
{
    // depth, width, height
    return size.z * size.x * size.y;
}

int VSChunk::getActiveBlockCount() const
{
    // depth, width, height
    return drawnBlocksOffsets.size();
}

glm::vec3 VSChunk::getSize() const
{
    return size;
}

void VSChunk::draw(VSWorld* world, std::shared_ptr<VSShader> shader) const
{
    shader->uniforms()
        .setVec3("lightPos", world->getDirectLightPos())
        .setVec3("lightColor", world->getDirectLightColor())
        .setVec3("viewPos", world->getCamera()->getPosition())
        .setVec3("chunkSize", getSize())
        .setMat4("model", getModelMatrix())
        .setMat4("MVP", world->getCamera()->getMVPMatrixFast(getModelMatrix()));

    // dont draw if no blocks active
    if (!drawnBlocksOffsets.empty())
    {
        glBindVertexArray(vertexContext->vertexArrayObject);
        glDrawElementsInstanced(
            GL_TRIANGLES,
            vertexContext->triangleCount,
            GL_UNSIGNED_INT,
            nullptr,
            drawnBlocksOffsets.size());
        glBindVertexArray(0);
    }
}

glm::mat4 VSChunk::getModelMatrix() const
{
    return modelMatrix;
}

void VSChunk::setModelMatrix(const glm::mat4& mat)
{
    modelMatrix = mat;
}

void VSChunk::setShouldDrawBorderBlocks(bool state)
{
    if (state != bShouldDrawBorderBlocks)
    {
        bShouldDrawBorderBlocks = state;
    }
}

void VSChunk::updateBlock(int blockIndex)
{
    // only update active blocks list if there is a visible change
    if (blocks[blockIndex] != defaultID && isBlockVisible(blockIndex)) {
        const auto offset = blockIndexToBlockCoordsFloat(blockIndex) - glm::vec3(size) / 2.f;
        drawnBlocksOffsets.push_back(offset);
    }
}

glm::ivec3 VSChunk::blockIndexToBlockCoords(int blockIndex) const
{
    const int width = size.x;
    const int depth = size.z;
    const int height = size.y;

    int x = blockIndex % width;
    int z = blockIndex / (width * height);
    int y = (blockIndex / width) % height;
    return {x, y, z};
}

glm::vec3 VSChunk::blockIndexToBlockCoordsFloat(int blockIndex) const
{
    const int width = size.x;
    const int depth = size.z;
    const int height = size.y;

    int x = blockIndex % width;
    int z = blockIndex / (width * height);
    int y = (blockIndex / width) % height;
    return {x, y, z};
}

int VSChunk::blockCoordsToBlockIndex(const glm::ivec3& blockCoords) const
{
    const int width = size.x;
    const int depth = size.z;
    const int height = size.y;

    return blockCoords.x + blockCoords.y * width + blockCoords.z * width * height;
}

bool VSChunk::isBlockVisible(int blockIndex) const
{
    const auto blockCoords = blockIndexToBlockCoords(blockIndex);
    const auto size = getSize();

    // if we are at the chunk border always visible
    if (isAtBorder(blockCoords))
    {
        if (!bShouldDrawBorderBlocks) {
            const auto top = glm::ivec3(blockCoords.x, blockCoords.y + 1, blockCoords.z);
            if (blocks[blockCoordsToBlockIndex(top)] != defaultID) {
                return false;
            }
        }
        return true;
    }

    const auto right = glm::ivec3(blockCoords.x + 1, blockCoords.y, blockCoords.z);
    const auto left = glm::ivec3(blockCoords.x - 1, blockCoords.y, blockCoords.z);

    const auto top = glm::ivec3(blockCoords.x, blockCoords.y + 1, blockCoords.z);
    const auto down = glm::ivec3(blockCoords.x, blockCoords.y - 1, blockCoords.z);

    const auto front = glm::ivec3(blockCoords.x, blockCoords.y, blockCoords.z + 1);
    // TODO is + 1 or -1 back?
    const auto back = glm::ivec3(blockCoords.x, blockCoords.y, blockCoords.z - 1);

    return blocks[blockCoordsToBlockIndex(right)] == defaultID ||
           blocks[blockCoordsToBlockIndex(left)] == defaultID ||
           blocks[blockCoordsToBlockIndex(top)] == defaultID ||
           blocks[blockCoordsToBlockIndex(down)] == defaultID ||
           blocks[blockCoordsToBlockIndex(front)] == defaultID ||
           blocks[blockCoordsToBlockIndex(back)] == defaultID;
}

bool VSChunk::isAtBorder(const glm::ivec3& blockCoords) const
{
    return blockCoords.x == 0 || blockCoords.x == size.x - 1 || blockCoords.y == 0 ||
           blockCoords.y == size.y - 1 || blockCoords.z == 0 || blockCoords.z == size.z - 1;
}
