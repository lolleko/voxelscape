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
    VSChunk(size_t width, size_t depth, size_t height, VSBlockID defaultID)
        : width(width)
        , depth(depth)
        , height(height)
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
        return width * depth * height;
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
    size_t width;
    size_t depth;
    size_t height;

    std::unique_ptr<VSVertexContext> vertexContext;

    VSBlockID* blocks;
};