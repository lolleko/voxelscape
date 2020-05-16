#include "core/vs_debug_draw.h"

#include "core/vs_camera.h"

#include "renderer/vs_shader.h"

#include "world/vs_world.h"

VSDebugDraw::VSDebugDraw()
{
    primitiveShader = std::make_shared<VSShader>("DebugDraw");
    vertexData.resize(maxVertexCount);

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
    vertexData.push_back({{box.mins.x, box.mins.y, box.mins.z}, color});
    vertexData.push_back({{box.maxs.x, box.mins.y, box.mins.z}, color});
    vertexData.push_back({{box.maxs.x, box.mins.y, box.maxs.z}, color});
    vertexData.push_back({{box.mins.x, box.mins.y, box.maxs.z}, color});
    topRectangle.vertexCount = vertexData.size() - topRectangle.startIndex;
    topRectangle.thickness = thickness;
    topRectangle.primitiveMode = GL_LINE_LOOP;

    VSDebugPrimitive botRectangle;
    botRectangle.startIndex = vertexData.size();
    vertexData.push_back({{box.maxs.x, box.maxs.y, box.maxs.z}, color});
    vertexData.push_back({{box.mins.x, box.maxs.y, box.maxs.z}, color});
    vertexData.push_back({{box.mins.x, box.maxs.y, box.mins.z}, color});
    vertexData.push_back({{box.maxs.x, box.maxs.y, box.mins.z}, color});
    botRectangle.vertexCount = vertexData.size() - botRectangle.startIndex;
    botRectangle.thickness = thickness;
    botRectangle.primitiveMode = GL_LINE_LOOP;

    VSDebugPrimitive connections;
    connections.startIndex = vertexData.size();
    vertexData.push_back({{box.maxs.x, box.maxs.y, box.maxs.z}, color});
    vertexData.push_back({{box.maxs.x, box.mins.y, box.maxs.z}, color});

    vertexData.push_back({{box.maxs.x, box.maxs.y, box.mins.z}, color});
    vertexData.push_back({{box.maxs.x, box.mins.y, box.mins.z}, color});

    vertexData.push_back({{box.mins.x, box.maxs.y, box.maxs.z}, color});
    vertexData.push_back({{box.mins.x, box.mins.y, box.maxs.z}, color});

    vertexData.push_back({{box.mins.x, box.maxs.y, box.mins.z}, color});
    vertexData.push_back({{box.mins.x, box.mins.y, box.mins.z}, color});
    connections.vertexCount = vertexData.size() - connections.startIndex;
    connections.thickness = thickness;
    connections.primitiveMode = GL_LINES;


    primitives.push_back(topRectangle);
    primitives.push_back(botRectangle);
    primitives.push_back(connections);
}

void VSDebugDraw::draw(VSWorld* world) const
{
    (void)world;
    primitiveShader->uniforms().setMat4("VP", world->getCamera()->getVPMatrix());

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        maxVertexCount * sizeof(VSDebugVertexData),
        &vertexData[0],
        GL_STATIC_DRAW);

    for (const auto& primitive : primitives)
    {
        drawPrimitive(primitive);
    }

    glBindVertexArray(0);

    // TODO never never never cons_cast
    const_cast<VSDebugDraw*>(this)->primitives.clear();
    const_cast<VSDebugDraw*>(this)->vertexData.clear();
}


void VSDebugDraw::drawPrimitive(const VSDebugDraw::VSDebugPrimitive& primitive) const
{
    glDrawArrays(primitive.primitiveMode, primitive.startIndex, primitive.vertexCount);
}

