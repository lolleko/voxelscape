#pragma once

#include <glad/glad.h>

#include <glm/vec3.hpp>
#include <vector>
#include <set>
#include <glm/vector_relational.hpp>

#include "renderer/vs_drawable.h"
#include "core/vs_box.h"

class VSDebugDraw : public IVSDrawable
{

public:
    VSDebugDraw();

    void drawBox(const VSBox& box, glm::vec<3, std::byte> color, float thickness = 1.f);

    void drawSphere(const glm::vec3& center, float radius, const glm::vec<3, std::byte>& color, std::uint32_t segments = 12, float thickness = 1.f);

    void draw(VSWorld* world) const override;

private:
    struct VSDebugPrimitive
    {
        float thickness = 1.f;
        GLuint primitiveMode = GL_LINES;
        std::size_t  startIndex;
        std::size_t  vertexCount;
    };

    std::vector<VSDebugPrimitive> primitives;

    std::shared_ptr<VSShader> primitiveShader;

    GLuint vertexArrayObject = 0;

    struct VSDebugVertexData {
        glm::vec3 position;
        glm::vec<3, std::byte> color;
    };

    std::vector<VSDebugVertexData> vertexData;

    GLuint vertexBuffer;

    void drawPrimitive(const VSDebugPrimitive& primitive) const;

    void addPrimitiveVertices(VSDebugPrimitive& inPrimitive, const std::vector<VSDebugVertexData>& vertices);
};