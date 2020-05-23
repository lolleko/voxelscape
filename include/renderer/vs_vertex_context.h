#pragma once

#include <glad/glad.h>

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <vector>

struct VSVertexData
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 biTangent;
    glm::vec3 color;
};

// PDO does not ocntain any functionality except cosntruct/desctruct
// should only be passed via unique ptr
struct VSVertexContext
{
public:
    GLuint vertexArrayObject = 0;
    GLuint vertexBuffer = 0;
    GLuint indexBuffer = 0;

    GLint lastAttribPointer = 0;

    GLint indexCount = 0;

    VSVertexContext(VSVertexContext const&) = delete;
    VSVertexContext& operator=(VSVertexContext const&) = delete;

    VSVertexContext(
        const std::vector<VSVertexData>& vertexData,
        const std::vector<GLuint>& triangleIndices)
    {
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        // vertex data buffer
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertexData.size() * sizeof(VSVertexData),
            &vertexData[0],
            GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(lastAttribPointer);
        glVertexAttribPointer(
            lastAttribPointer,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(VSVertexData),
            (void*)offsetof(VSVertexData, position));

        // vertex normals
        lastAttribPointer++;
        glEnableVertexAttribArray(lastAttribPointer);
        glVertexAttribPointer(
            lastAttribPointer,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(VSVertexData),
            (void*)offsetof(VSVertexData, normal));

        // vertex texCords
        lastAttribPointer++;
        glEnableVertexAttribArray(lastAttribPointer);
        glVertexAttribPointer(
            lastAttribPointer,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(VSVertexData),
            (void*)offsetof(VSVertexData, texCoords));

        // vertex tangents
        lastAttribPointer++;
        glEnableVertexAttribArray(lastAttribPointer);
        glVertexAttribPointer(
            lastAttribPointer,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(VSVertexData),
            (void*)offsetof(VSVertexData, tangent));

        // vertex bitangents
        lastAttribPointer++;
        glEnableVertexAttribArray(lastAttribPointer);
        glVertexAttribPointer(
            lastAttribPointer,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(VSVertexData),
            (void*)offsetof(VSVertexData, biTangent));

        // vertex colors
        lastAttribPointer++;
        glEnableVertexAttribArray(lastAttribPointer);
        glVertexAttribPointer(
            lastAttribPointer,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(VSVertexData),
            (void*)offsetof(VSVertexData, color));

        // triangle indices
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            triangleIndices.size() * sizeof(GLuint),
            &triangleIndices[0],
            GL_STATIC_DRAW);

        indexCount = triangleIndices.size();

        glBindVertexArray(0);
    };

    ~VSVertexContext()
    {
        glBindVertexArray(vertexArrayObject);
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &indexBuffer);
        glDeleteVertexArrays(1, &vertexArrayObject);
    };
};