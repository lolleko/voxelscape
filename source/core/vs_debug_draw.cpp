#include "core/vs_debug_draw.h"
#include <concurrentqueue/concurrentqueue.h>

#include <cstdint>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
#include <vector>

constexpr double PI = 3.14159265358979323846;

#include "core/vs_camera.h"
#include "renderer/vs_shader.h"
#include "world/vs_world.h"

VSDebugDraw::VSDebugDraw()
{
    primitiveShader = std::make_shared<VSShader>("DebugDraw");

    primitives = moodycamel::ConcurrentQueue<VSDebugPrimitive>(256);

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
    VSDebugPrimitive boxPrimitive;

    boxPrimitive.vertices.insert(
        boxPrimitive.vertices.end(),
        {// Top
         {{box.mins.x, box.mins.y, box.mins.z}, color},
         {{box.maxs.x, box.mins.y, box.mins.z}, color},

         {{box.maxs.x, box.mins.y, box.mins.z}, color},
         {{box.maxs.x, box.mins.y, box.maxs.z}, color},

         {{box.maxs.x, box.mins.y, box.maxs.z}, color},
         {{box.mins.x, box.mins.y, box.maxs.z}, color},

         {{box.mins.x, box.mins.y, box.maxs.z}, color},
         {{box.mins.x, box.mins.y, box.mins.z}, color},

         // botom
         {{box.mins.x, box.maxs.y, box.mins.z}, color},
         {{box.maxs.x, box.maxs.y, box.mins.z}, color},

         {{box.maxs.x, box.maxs.y, box.mins.z}, color},
         {{box.maxs.x, box.maxs.y, box.maxs.z}, color},

         {{box.maxs.x, box.maxs.y, box.maxs.z}, color},
         {{box.mins.x, box.maxs.y, box.maxs.z}, color},

         {{box.mins.x, box.maxs.y, box.maxs.z}, color},
         {{box.mins.x, box.maxs.y, box.mins.z}, color},

         // connections
         {{box.maxs.x, box.maxs.y, box.maxs.z}, color},
         {{box.maxs.x, box.mins.y, box.maxs.z}, color},
         {{box.maxs.x, box.maxs.y, box.mins.z}, color},
         {{box.maxs.x, box.mins.y, box.mins.z}, color},
         {{box.mins.x, box.maxs.y, box.maxs.z}, color},
         {{box.mins.x, box.mins.y, box.maxs.z}, color},
         {{box.mins.x, box.maxs.y, box.mins.z}, color},
         {{box.mins.x, box.mins.y, box.mins.z}, color}});
    boxPrimitive.thickness = thickness;
    boxPrimitive.primitiveMode = GL_LINES;

    primitives.enqueue(boxPrimitive);
}

void VSDebugDraw::drawLine(
    const glm::vec3& start,
    const glm::vec3& end,
    glm::vec<3, std::byte> color,
    float thickness)
{
    VSDebugPrimitive line;
    line.vertices.insert(line.vertices.end(), {{start, color}, {end, color}});
    line.thickness = thickness;
    line.primitiveMode = GL_LINES;

    primitives.enqueue(line);
}

void VSDebugDraw::drawFrustum(const glm::mat4& VP, glm::vec<3, std::byte> color, float thickness)
{
    glm::vec3 vertices[2][2][2];
    const auto VPToWorld = glm::inverse(VP);
    for (std::uint32_t Z = 0; Z < 2; Z++)
    {
        for (std::uint32_t Y = 0; Y < 2; Y++)
        {
            for (std::uint32_t X = 0; X < 2; X++)
            {
                const auto unprojectedVertex =
                    VPToWorld *
                    glm::vec4((X ? -1.0f : 1.0f), (Y ? -1.0f : 1.0f), (Z ? 0.0f : 1.0f), 1.0f);
                vertices[X][Y][Z] = glm::vec3(unprojectedVertex) / unprojectedVertex.w;
            }
        }
    }

    VSDebugPrimitive frustumPrimitive;
    frustumPrimitive.thickness = thickness;
    frustumPrimitive.primitiveMode = GL_LINES;
    frustumPrimitive.vertices.insert(
        frustumPrimitive.vertices.end(),
        {
            {vertices[0][0][0], color}, {vertices[0][0][1], color}, {vertices[1][0][0], color},
            {vertices[1][0][1], color}, {vertices[0][1][0], color}, {vertices[0][1][1], color},
            {vertices[1][1][0], color}, {vertices[1][1][1], color},

            {vertices[0][0][0], color}, {vertices[0][1][0], color}, {vertices[1][0][0], color},
            {vertices[1][1][0], color}, {vertices[0][0][1], color}, {vertices[0][1][1], color},
            {vertices[1][0][1], color}, {vertices[1][1][1], color},

            {vertices[0][0][0], color}, {vertices[1][0][0], color}, {vertices[0][1][0], color},
            {vertices[1][1][0], color}, {vertices[0][0][1], color}, {vertices[1][0][1], color},
            {vertices[0][1][1], color}, {vertices[1][1][1], color},
        });

    primitives.enqueue(frustumPrimitive);
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
    const float angleInc = 2.f * PI / float(segments);
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
    sphere.thickness = thickness;
    sphere.primitiveMode = GL_LINES;

    while ((numSegmentsY--) != 0)
    {
        sinY2 = glm::sin(latitude);
        cosY2 = glm::cos(latitude);

        vertex1 = glm::vec3(sinY1, cosY1, 0.F) * radius + center;
        vertex3 = glm::vec3(sinY2, cosY2, 0.F) * radius + center;
        longitude = angleInc;

        numSegmentsX = segments;
        while ((numSegmentsX--) != 0)
        {
            sinX = glm::sin(longitude);
            cosX = glm::cos(longitude);

            vertex2 = glm::vec3((cosX * sinY1), cosY1, (sinX * sinY1)) * radius + center;
            vertex4 = glm::vec3((cosX * sinY2), cosY2, (sinX * sinY2)) * radius + center;

            sphere.vertices.insert(sphere.vertices.end(), {{vertex1, color}, {vertex2, color}});
            sphere.vertices.insert(sphere.vertices.end(), {{vertex1, color}, {vertex3, color}});

            vertex1 = vertex2;
            vertex3 = vertex4;
            longitude += angleInc;
        }
        sinY1 = sinY2;
        cosY1 = cosY2;
        latitude += angleInc;
    }

    primitives.enqueue(sphere);
}

void VSDebugDraw::drawPoint(
    const glm::vec3& center,
    const glm::vec<3, std::byte>& color,
    float thickness)
{
    VSDebugPrimitive point;
    point.vertices.push_back({center, color});
    point.thickness = thickness;
    point.primitiveMode = GL_POINTS;

    primitives.enqueue(point);
}

void VSDebugDraw::draw(VSWorld* world)
{
    (void)world;
    primitiveShader->uniforms().setMat4("VP", world->getCamera()->getVPMatrix());

    // Get current set of vertices
    const auto primitveApproxCount = primitives.size_approx();
    std::vector<VSDebugPrimitive> currentPrimitives(primitveApproxCount);
    size_t primitiveActualcount =
        primitives.try_dequeue_bulk(currentPrimitives.begin(), primitveApproxCount);
    for (size_t i = 0; i != primitiveActualcount; ++i)
    {
        auto& currentPrimitive = currentPrimitives[i];
        currentPrimitive.startIndex = vertexData.size();
        vertexData.insert(
            vertexData.end(), currentPrimitive.vertices.begin(), currentPrimitive.vertices.end());
    }

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertexData.size() * sizeof(VSDebugVertexData),
        vertexData.data(),
        GL_STATIC_DRAW);

    for (size_t i = 0; i != primitiveActualcount; ++i)
    {
        drawPrimitive(currentPrimitives[i]);
    }

    glBindVertexArray(0);

    vertexData.clear();
}

void VSDebugDraw::drawPrimitive(const VSDebugDraw::VSDebugPrimitive& primitive) const
{
    if (primitive.primitiveMode == GL_POINTS)
    {
        glPointSize(primitive.thickness);
    }
    else
    {
        glLineWidth(primitive.thickness);
    }
    glDrawArrays(primitive.primitiveMode, primitive.startIndex, primitive.vertices.size());
}
