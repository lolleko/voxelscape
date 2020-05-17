#include "core/vs_debug_draw.h"

#include "core/vs_camera.h"

#include "renderer/vs_shader.h"

#include "world/vs_world.h"

#define _USE_MATH_DEFINES
#include <cmath>

VSDebugDraw::VSDebugDraw()
{
    primitiveShader = std::make_shared<VSShader>("DebugDraw");

    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VSDebugVertexData),
        (GLvoid*)offsetof(VSDebugVertexData, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_UNSIGNED_BYTE,
        GL_TRUE,
        sizeof(VSDebugVertexData),
        (GLvoid*)offsetof(VSDebugVertexData, color));

    glBindVertexArray(0);
}

void VSDebugDraw::drawBox(const VSBox& box, glm::vec<3, std::byte> color, float thickness)
{
    VSDebugPrimitive topRectangle;
    topRectangle.startIndex = vertexData.size();
    addPrimitiveVertices(
        topRectangle,
        {{{box.mins.x, box.mins.y, box.mins.z}, color},
         {{box.maxs.x, box.mins.y, box.mins.z}, color},
         {{box.maxs.x, box.mins.y, box.maxs.z}, color},
         {{box.mins.x, box.mins.y, box.maxs.z}, color}});
    topRectangle.thickness = thickness;
    topRectangle.primitiveMode = GL_LINE_LOOP;

    VSDebugPrimitive botRectangle;
    botRectangle.startIndex = vertexData.size();
    addPrimitiveVertices(
        botRectangle,
        {{{box.maxs.x, box.maxs.y, box.maxs.z}, color},
         {{box.mins.x, box.maxs.y, box.maxs.z}, color},
         {{box.mins.x, box.maxs.y, box.mins.z}, color},
         {{box.maxs.x, box.maxs.y, box.mins.z}, color}});
    botRectangle.thickness = thickness;
    botRectangle.primitiveMode = GL_LINE_LOOP;

    VSDebugPrimitive connections;
    connections.startIndex = vertexData.size();
    addPrimitiveVertices(
        connections,
        {{{box.maxs.x, box.maxs.y, box.maxs.z}, color},
         {{box.maxs.x, box.mins.y, box.maxs.z}, color},
         {{box.maxs.x, box.maxs.y, box.mins.z}, color},
         {{box.maxs.x, box.mins.y, box.mins.z}, color},
         {{box.mins.x, box.maxs.y, box.maxs.z}, color},
         {{box.mins.x, box.mins.y, box.maxs.z}, color},
         {{box.mins.x, box.maxs.y, box.mins.z}, color},
         {{box.mins.x, box.mins.y, box.mins.z}, color}});
    connections.thickness = thickness;
    connections.primitiveMode = GL_LINES;

    primitives.push_back(topRectangle);
    primitives.push_back(botRectangle);
    primitives.push_back(connections);
}

void VSDebugDraw::drawSphere(
    const glm::vec3& center,
    float radius,
    const glm::vec<3, std::byte>& color,
    std::uint32_t segments,
    float thickness)
{
    glm::vec3 vertex1;
    glm::vec3 vertex2;
    glm::vec3 vertex3;
    glm::vec3 vertex4;
    const float angleInc = 2.f * M_PI / float(segments);
    std::int32_t numSegmentsY = segments;
    float latitude = angleInc;
    std::int32_t numSegmentsX;
    float longitude;
    float sinY1 = 0.0f;
    float cosY1 = 1.0f;
    float sinY2;
    float cosY2;
    float sinX;
    float cosX;

    VSDebugPrimitive sphere;
    sphere.startIndex = vertexData.size();
    sphere.thickness = thickness;
    sphere.primitiveMode = GL_LINES;

    while ((numSegmentsY--) != 0)
    {
        sinY2 = glm::sin(latitude);
        cosY2 = glm::cos(latitude);

        vertex1 = glm::vec3(sinY1, cosY1, 0.f) * radius + center;
        vertex3 = glm::vec3(sinY2, cosY2, 0.f) * radius + center;
        longitude = angleInc;

        numSegmentsX = segments;
        while ((numSegmentsX--) != 0)
        {
            sinX = glm::sin(longitude);
            cosX = glm::cos(longitude);

            vertex2 = glm::vec3((cosX * sinY1), cosY1, (sinX * sinY1)) * radius + center;
            vertex4 = glm::vec3((cosX * sinY2), cosY2, (sinX * sinY2)) * radius + center;

            addPrimitiveVertices(sphere, {{vertex1, color}, {vertex2, color}});
            addPrimitiveVertices(sphere, {{vertex1, color}, {vertex3, color}});

            vertex1 = vertex2;
            vertex3 = vertex4;
            longitude += angleInc;
        }
        sinY1 = sinY2;
        cosY1 = cosY2;
        latitude += angleInc;
    }

    primitives.push_back(sphere);
}

void VSDebugDraw::draw(VSWorld* world)
{
    (void)world;
    primitiveShader->uniforms().setMat4("VP", world->getCamera()->getVPMatrix());

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertexData.size() * sizeof(VSDebugVertexData),
        &vertexData[0],
        GL_STATIC_DRAW);

    for (const auto& primitive : primitives)
    {
        drawPrimitive(primitive);
    }

    glBindVertexArray(0);

    primitives.clear();
    vertexData.clear();
}

void VSDebugDraw::drawPrimitive(const VSDebugDraw::VSDebugPrimitive& primitive) const
{
    glLineWidth(primitive.thickness);
    glDrawArrays(primitive.primitiveMode, primitive.startIndex, primitive.vertexCount);
}

void VSDebugDraw::addPrimitiveVertices(
    VSDebugPrimitive& inPrimitive,
    const std::vector<VSDebugVertexData>& vertices)
{
    vertexData.insert(vertexData.end(), vertices.begin(), vertices.end());
    inPrimitive.vertexCount = vertexData.size() - inPrimitive.startIndex;
}
