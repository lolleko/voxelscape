#pragma once

#include "glad/glad.h"

#include <glm/glm.hpp>

#include <array>

// TODO move implementation to cube.cpp

class VSCube
{
public:
    VSCube()
    {
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        // vertex positions
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

        // vertex normals
        glGenBuffers(1, &normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

        // vertex colors
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);

        // triangle indices
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLubyte), &indices[0], GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    ~VSCube()
    {
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &normalBuffer);
        glDeleteBuffers(1, &colorBuffer);
        glDeleteBuffers(1, &indexBuffer);
        glDeleteVertexArrays(1, &vertexArrayObject);
    }

    void draw() const
    {
        glBindVertexArray(vertexArrayObject);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_BYTE, nullptr);
        glBindVertexArray(0);
    }

    glm::mat4 getLocalToWorld()
    {
        return localTransform;
    }

private:
    glm::mat4 localTransform = glm::mat4(1.0f);

    static inline const auto vertexCount = 8;

    static inline const auto vertices = std::array<glm::vec3, vertexCount>{
        glm::vec3(-1, -1, -1),
        glm::vec3(1, -1, -1),
        glm::vec3(1, 1, -1),
        glm::vec3(-1, 1, -1),
        glm::vec3(-1, -1, 1),
        glm::vec3(1, -1, 1),
        glm::vec3(1, 1, 1),
        glm::vec3(-1, 1, 1)};

    static inline const auto normals = std::array<glm::vec3, 6>{
        glm::vec3(0, 0, 1),
        glm::vec3(1, 0, 0),
        glm::vec3(0, 0, -1),
        glm::vec3(-1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, -1, 0)};

    static inline const auto colors = std::array<glm::vec3, vertexCount>{
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0),
        glm::vec3(0.0, 0.0, 1.0),
        glm::vec3(1.0, 1.0, 1.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0),
        glm::vec3(0.0, 0.0, 1.0),
        glm::vec3(1.0, 1.0, 1.0)};

    static inline const auto indices =
        std::array<GLubyte, 36>{0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6, 5, 4, 6, 6, 4, 7,
                                4, 0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1};

    GLuint vertexArrayObject, vertexBuffer, normalBuffer, colorBuffer, indexBuffer;
};