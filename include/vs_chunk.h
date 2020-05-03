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

        // active blocks size always has to be equal to to totalBlockCount
        activeBlockOffsets.resize(getTotalBlockCount());

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
        activeBlockCount = 0;
        for (size_t blockIndex = 0; blockIndex < getTotalBlockCount(); blockIndex++)
        {
            // std::random_device rd;
            // std::mt19937 mt(rd());
            // std::uniform_int_distribution<int> uniformDist(0, 1);
            if (blocks[blockIndex] != defaultID)
            {
                int W = size.x;
                int D = size.z;
                int H = size.y;

                int x = blockIndex % W;
                int z = blockIndex / (W * H);
                int y = (blockIndex / W) % H;

                // subtract size / 2 to center around 0, 0, 0
                const auto offset = glm::vec3(x, y, z) - size / 2.f;
                activeBlockOffsets[activeBlockCount] = glm::vec3(x - W / 2, y - H / 2, z - D / 2);
                activeBlockCount++;
            }
        }

        glBindVertexArray(vertexContext->vertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, activeBlocksInstanceBuffer);
        glBufferSubData(
            GL_ARRAY_BUFFER,
            0,
            activeBlockOffsets.size() * sizeof(glm::vec3),
            &activeBlockOffsets[0]);

        glBindVertexArray(0);
    };

    void setBlock(glm::ivec3 location, VSBlockID blockID)
    {
        int W = size.x;
        int D = size.z;
        int H = size.y;

        int blockIndex = location.x + location.y * W + location.z * W * H;

        assert(blockIndex < getTotalBlockCount());

        blocks[blockIndex] = blockID;
    }

    size_t getTotalBlockCount() const
    {
        // depth, width, height
        return size.z * size.x * size.y;
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
        if (activeBlockCount != 0)
        {
            glBindVertexArray(vertexContext->vertexArrayObject);
            glDrawElementsInstanced(
                GL_TRIANGLES,
                vertexContext->triangleCount,
                GL_UNSIGNED_INT,
                nullptr,
                activeBlockCount);
            glBindVertexArray(0);
        }
    };

    glm::mat4 getModelMatrix() const override
    {
        return glm::mat4(1.f);
    }

private:
    glm::vec3 size;

    VSBlockID defaultID;

    std::unique_ptr<VSVertexContext> vertexContext;

    GLuint activeBlocksInstanceBuffer;

    std::vector<glm::vec3> activeBlockOffsets;

    int activeBlockCount;

    VSBlockID* blocks;
};