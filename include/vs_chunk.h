#pragma once

#include <cstddef>
#include <memory>
#include "vs_block.h"
#include "vs_drawable.h"
#include "vs_modelloader.h"
#include "vs_vertex_context.h"

class VSChunk : public IVSDrawable
{
public:
    // Size can only be set on construction
    VSChunk(const glm::vec3& size, VSBlockID defaultID)
        : size(size)
    {
        blocks = new VSBlockID[getBlockCount()];

        for (size_t i = 0; i < getBlockCount(); i++)
        {
            blocks[i] = defaultID;
        }

        vertexContext = loadVertexContext("cube.obj");
    }

    size_t getBlockCount() const
    {
        // depth, width, height
        return size.z * size.x * size.y;
    }

    glm::vec3 getSize() const
    {
        return size;
    }

    void draw(std::shared_ptr<VSShader> shader) const override
    {
        shader->use();
        glBindVertexArray(vertexContext->vertexArrayObject);
        glDrawElementsInstanced(
            GL_TRIANGLES, vertexContext->triangleCount, GL_UNSIGNED_INT, nullptr, getBlockCount());
        glBindVertexArray(0);
    };

private:
    glm::vec3 size;

    std::unique_ptr<VSVertexContext> vertexContext;

    VSBlockID* blocks;
};