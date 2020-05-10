#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <memory>
#include <utility>
#include <vector>
#include <string>
#include "renderer/vs_shader.h"
#include "renderer/vs_vertex_context.h"
#include "renderer/vs_drawable.h"

// Roughly based on https://learnopengl.com/Model-Loading/Mesh

struct VSTexture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class VSMesh : public IVSDrawable
{
public:
    VSMesh(VSVertexContext* vertexContext, std::vector<VSTexture> textures);

    ~VSMesh();

    void draw(VSWorld* world, std::shared_ptr<VSShader> shader) const override;

private:
    VSVertexContext* vertexContext;
    std::vector<VSTexture> textures;
};
