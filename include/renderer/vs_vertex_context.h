#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <vector>

// PDO does not ocntain any functionality except cosntruct/desctruct
// should only be passed via unique ptr
struct VSVertexContext
{
public:
    GLuint vertexArrayObject = 0;
    GLuint vertexBuffer = 0;
    GLuint normalBuffer = 0;
    GLuint tangentBuffer = 0;
    GLuint biTangentBuffer = 0;
    GLuint colorBuffer = 0;
    GLuint texCoordBuffer = 0;
    GLuint indexBuffer = 0;

    GLint lastAttribPointer = 0;

    GLint triangleCount = 0;

    VSVertexContext(VSVertexContext const&) = delete;
    VSVertexContext& operator=(VSVertexContext const&) = delete;

    VSVertexContext(
        std::vector<glm::vec3> vertexPositions,
        std::vector<glm::vec3> vertexNormals,
        std::vector<glm::vec2> vertexTexCoords,
        std::vector<glm::vec3> vertexTangents,
        std::vector<glm::vec3> vertexBiTangents,
        std::vector<glm::vec3> vertexColors,
        std::vector<GLuint> triangleIndices)
    {
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        // vertex positions
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glEnableVertexAttribArray(lastAttribPointer);
        glVertexAttribPointer(lastAttribPointer, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertexPositions.size() * sizeof(glm::vec3),
            &vertexPositions[0],
            GL_STATIC_DRAW);

        // vertex normals
        lastAttribPointer++;
        glGenBuffers(1, &normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glEnableVertexAttribArray(lastAttribPointer);
        glVertexAttribPointer(lastAttribPointer, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertexNormals.size() * sizeof(glm::vec3),
            &vertexNormals[0],
            GL_STATIC_DRAW);

        // vertex texCords
        lastAttribPointer++;
        glGenBuffers(1, &texCoordBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
        glEnableVertexAttribArray(lastAttribPointer);
        glVertexAttribPointer(lastAttribPointer, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertexTexCoords.size() * sizeof(glm::vec2),
            &vertexTexCoords[0],
            GL_STATIC_DRAW);

        // vertex tangents
        lastAttribPointer++;
        glGenBuffers(1, &tangentBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
        glEnableVertexAttribArray(lastAttribPointer);
        glVertexAttribPointer(lastAttribPointer, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertexTangents.size() * sizeof(glm::vec3),
            &vertexTangents[0],
            GL_STATIC_DRAW);

        // vertex bitangents
        lastAttribPointer++;
        glGenBuffers(1, &biTangentBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, biTangentBuffer);
        glEnableVertexAttribArray(lastAttribPointer);
        glVertexAttribPointer(lastAttribPointer, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertexBiTangents.size() * sizeof(glm::vec3),
            &vertexBiTangents[0],
            GL_STATIC_DRAW);

        // vertex colors
        lastAttribPointer++;
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glEnableVertexAttribArray(lastAttribPointer);
        glVertexAttribPointer(lastAttribPointer, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertexColors.size() * sizeof(glm::vec3),
            &vertexColors[0],
            GL_STATIC_DRAW);

        // triangle indices
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            triangleIndices.size() * sizeof(GLuint),
            &triangleIndices[0],
            GL_STATIC_DRAW);

        triangleCount = triangleIndices.size();

        glBindVertexArray(0);
    };

    ~VSVertexContext()
    {
        glBindVertexArray(vertexArrayObject);
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &normalBuffer);
        glDeleteBuffers(1, &texCoordBuffer);
        glDeleteBuffers(1, &tangentBuffer);
        glDeleteBuffers(1, &biTangentBuffer);
        glDeleteBuffers(1, &colorBuffer);
        glDeleteBuffers(1, &indexBuffer);
        glDeleteVertexArrays(1, &vertexArrayObject);
    };
};