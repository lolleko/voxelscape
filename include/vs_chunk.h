#pragma once

#include <cstddef>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <random>
#include "vs_block.h"
#include "vs_drawable.h"
#include "vs_modelloader.h"
#include "vs_vertex_context.h"
#include "vs_transformable.h"
#include "vs_world.h"

class VSChunk : public IVSDrawable, IVSTransformable
{
public:
    // Size can only be set on construction
    VSChunk(const glm::vec3& size, VSBlockID defaultID)
        : size(size)
        , defaultID(defaultID)
    {
        blocks = new VSBlockID[getTotalBlockCount()];

        for (size_t blockIndex = 0; blockIndex < getTotalBlockCount(); blockIndex++)
        {
            blocks[blockIndex] = defaultID;
        }

        vertexContext = loadVertexContext("cube.obj");

        glBindVertexArray(vertexContext->vertexArrayObject);

        const auto nextAttribPointer = vertexContext->lastAttribPointer + 1;
        glGenBuffers(1, &activeBlocksInstanceBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, activeBlocksInstanceBuffer);
        glEnableVertexAttribArray(nextAttribPointer);
        glVertexAttribPointer(nextAttribPointer, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER,
            activeBlockOffsets.size() * sizeof(glm::vec3),
            &activeBlockOffsets[0],
            GL_STATIC_DRAW);
        glVertexAttribDivisor(nextAttribPointer, 1);

        glBindVertexArray(0);

        updateActiveBlocks();
    }

    void updateActiveBlocks()
    {
        activeBlockOffsets.clear();
        for (size_t blockIndex = 0; blockIndex < getTotalBlockCount(); blockIndex++)
        {
            // std::random_device rd;
            // std::mt19937 mt(rd());
            // std::uniform_int_distribution<int> uniformDist(0, 1);
            if (blocks[blockIndex] != defaultID && isBlockVisible(blockIndex))
            {
                // subtract size / 2 to center around 0, 0, 0
                const auto offset = blockIndexToBlockCoordsFloat(blockIndex) - size / 2.f;
                activeBlockOffsets.push_back(offset);
            }
        }

        glBindVertexArray(vertexContext->vertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, activeBlocksInstanceBuffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            activeBlockOffsets.size() * sizeof(glm::vec3),
            &activeBlockOffsets[0],
            GL_STATIC_DRAW);

        glBindVertexArray(0);
    };

    void setBlock(glm::ivec3 location, VSBlockID blockID)
    {
        blocks[blockCoordsToBlockIndex(location)] = blockID;
    }

    int getTotalBlockCount() const
    {
        // depth, width, height
        return size.z * size.x * size.y;
    }

    int getActiveBlockCount() const
    {
        // depth, width, height
        return activeBlockOffsets.size();
    }

    glm::vec3 getSize() const
    {
        return size;
    }

    void draw(std::shared_ptr<VSWorld> world, std::shared_ptr<VSShader> shader) const override
    {
        shader->uniforms()
            .setVec3("lightPos", world->getDirectLightPos())
            .setVec3("lightColor", world->getDirectLightColor())
            .setVec3("viewPos", world->getCamera()->getPosition())
            .setVec3("chunkSize", getSize())
            .setMat4("model", getModelMatrix())
            .setMat4("MVP", world->getCamera()->getMVPMatrixFast(getModelMatrix()));

        // dont draw if no blocks active
        if (!activeBlockOffsets.empty())
        {
            glBindVertexArray(vertexContext->vertexArrayObject);
            glDrawElementsInstanced(
                GL_TRIANGLES,
                vertexContext->triangleCount,
                GL_UNSIGNED_INT,
                nullptr,
                activeBlockOffsets.size());
            glBindVertexArray(0);
        }
    };

    glm::mat4 getModelMatrix() const override
    {
        return glm::mat4(1.f);
    }

    void setShouldDrawBorderBlocks(bool state)
    {
        if (state != bShouldDrawBorderBlocks)
        {
            bShouldDrawBorderBlocks = state;
            updateActiveBlocks();
        }
    }

private:
    glm::vec3 size;

    VSBlockID defaultID;

    std::unique_ptr<VSVertexContext> vertexContext;

    GLuint activeBlocksInstanceBuffer;

    std::vector<glm::vec3> activeBlockOffsets;

    VSBlockID* blocks;

    bool bShouldDrawBorderBlocks = false;

    glm::ivec3 blockIndexToBlockCoords(int blockIndex)
    {
        const int width = size.x;
        const int depth = size.z;
        const int height = size.y;

        int x = blockIndex % width;
        int z = blockIndex / (width * height);
        int y = (blockIndex / width) % height;
        return {x, y, z};
    }

    glm::vec3 blockIndexToBlockCoordsFloat(int blockIndex)
    {
        const int width = size.x;
        const int depth = size.z;
        const int height = size.y;

        int x = blockIndex % width;
        int z = blockIndex / (width * height);
        int y = (blockIndex / width) % height;
        return {x, y, z};
    }

    int blockCoordsToBlockIndex(const glm::ivec3& blockCoords)
    {
        const int width = size.x;
        const int depth = size.z;
        const int height = size.y;

        return blockCoords.x + blockCoords.y * width + blockCoords.z * width * height;
    }

    bool isBlockVisible(int blockIndex)
    {
        const auto blockCoords = blockIndexToBlockCoords(blockIndex);
        const auto size = getSize();

        // if we are at the chunk border always render
        if (blockCoords.x == 0 || blockCoords.x == size.x - 1 || blockCoords.y == 0 ||
            blockCoords.y == size.y - 1 || blockCoords.z == 0 || blockCoords.z == size.z - 1)
        {
            return bShouldDrawBorderBlocks;
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
};

// const auto isRightVisible =
//     blockCoords.x > 0 && blockCoords.x < getSize().x &&
//     blocks[blockCoordsToBlockIndex(
//         glm::ivec3(blockCoords.x + 1, blockCoords.y, blockCoords.z))] != defaultID;
// const auto isLeftVisible =
//     blockCoords.x > 0 && blockCoords.x < getSize().x &&
//     blocks[blockCoordsToBlockIndex(
//         glm::ivec3(blockCoords.x - 1, blockCoords.y, blockCoords.z))] != defaultID;

// const auto isTopVisible =
//     blockCoords.y > 0 && blockCoords.y < getSize().y &&
//     blocks[blockCoordsToBlockIndex(
//         glm::ivec3(blockCoords.x, blockCoords.y + 1, blockCoords.z))] != defaultID;
// const auto isBottomVisible =
//     blockCoords.y > 0 && blockCoords.y < getSize().y &&
//     blocks[blockCoordsToBlockIndex(
//         glm::ivec3(blockCoords.x, blockCoords.y - 1, blockCoords.z))] != defaultID;

// const auto isFrontVisible =
//     blockCoords.z > 0 && blockCoords.z < getSize().z &&
//     blocks[blockCoordsToBlockIndex(
//         glm::ivec3(blockCoords.x + 1, blockCoords.y, blockCoords.z + 1))] != defaultID;
// // TODO is + 1 or -1 back?
// const auto isBackVisible =
//     blockCoords.z > 0 && blockCoords.z < getSize().z &&
//     blocks[blockCoordsToBlockIndex(
//         glm::ivec3(blockCoords.x, blockCoords.y, blockCoords.z - 1))] != defaultID;

// return !isRightVisible || !isLeftVisible || !isTopVisible || !isBottomVisible ||
//        !isFrontVisible || !isBackVisible;

// for (int componentIndex = 0; componentIndex < 3; componentIndex++)
// {
//     // if we are at the chunk border always render
//     if (blockCoords[componentIndex] == 0 ||
//         blockCoords[componentIndex] == getSize()[componentIndex])
//     {
//         return true;
//     }

//     for (int direction = 0; direction < 2; direction++)
//     {
//         auto neighbourCoords = glm::ivec3(blockCoords.x, blockCoords.y, blockCoords.z);
//         neighbourCoords[componentIndex] += (direction == 0 ? -1 : +1);
//     }
// }