#pragma once

#include <array>
#include <glm/fwd.hpp>
#include "renderer/vs_drawable.h"
#include "renderer/vs_shader.h"
#include "world/vs_transformable.h"

// Based on https://learnopengl.com/Advanced-OpenGL/Cubemaps
class VSSkybox : public IVSDrawable, IVSTransformable
{
public:
    VSSkybox();

    void draw(VSWorld* world, std::shared_ptr<VSShader> shader) const override;

    glm::mat4 getModelMatrix() const override;

    void setModelMatrix(const glm::mat4& mat) override;

private:
    const std::array<float, 108> skyboxVertices = {
        // positions
        -1.0F, 1.0F,  -1.0F, -1.0F, -1.0F, -1.0F, 1.0F,  -1.0F, -1.0F,
        1.0F,  -1.0F, -1.0F, 1.0F,  1.0F,  -1.0F, -1.0F, 1.0F,  -1.0F,

        -1.0F, -1.0F, 1.0F,  -1.0F, -1.0F, -1.0F, -1.0F, 1.0F,  -1.0F,
        -1.0F, 1.0F,  -1.0F, -1.0F, 1.0F,  1.0F,  -1.0F, -1.0F, 1.0F,

        1.0F,  -1.0F, -1.0F, 1.0F,  -1.0F, 1.0F,  1.0F,  1.0F,  1.0F,
        1.0F,  1.0F,  1.0F,  1.0F,  1.0F,  -1.0F, 1.0F,  -1.0F, -1.0F,

        -1.0F, -1.0F, 1.0F,  -1.0F, 1.0F,  1.0F,  1.0F,  1.0F,  1.0F,
        1.0F,  1.0F,  1.0F,  1.0F,  -1.0F, 1.0F,  -1.0F, -1.0F, 1.0F,

        -1.0F, 1.0F,  -1.0F, 1.0F,  1.0F,  -1.0F, 1.0F,  1.0F,  1.0F,
        1.0F,  1.0F,  1.0F,  -1.0F, 1.0F,  1.0F,  -1.0F, 1.0F,  -1.0F,

        -1.0F, -1.0F, -1.0F, -1.0F, -1.0F, 1.0F,  1.0F,  -1.0F, -1.0F,
        1.0F,  -1.0F, -1.0F, -1.0F, -1.0F, 1.0F,  1.0F,  -1.0F, 1.0F};

    unsigned int cubemapTexture;
    unsigned int skyboxVAO{}, skyboxVBO{};
};