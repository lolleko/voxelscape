#pragma once

#include <cstddef>
#include <memory>
#include "vs_block.h"
#include "vs_drawable.h"
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
    }

    size_t getBlockCount() const
    {
        return width * depth * height;
    }

    void draw(std::shared_ptr<VSShader> shader) const override{

    };

private:
    size_t width;
    size_t depth;
    size_t height;

    std::unique_ptr<VSVertexContext> vertexContext;

    VSBlockID* blocks;
};