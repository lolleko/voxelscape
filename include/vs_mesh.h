#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <memory>
#include <utility>
#include <vector>
#include <string>
#include "vs_shader.h"
#include "vs_vertex_context.h"
#include "vs_drawable.h"

struct VSTexture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class VSMesh : public IVSDrawable
{
public:
    VSMesh(std::unique_ptr<VSVertexContext> vertexContext, std::vector<VSTexture> textures);

    void draw(std::shared_ptr<VSWorld> world, std::shared_ptr<VSShader> shader) const override;

private:
    std::unique_ptr<VSVertexContext> vertexContext;
    std::vector<VSTexture> textures;
};
