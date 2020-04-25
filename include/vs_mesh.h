#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <utility>
#include <vector>
#include <string>
#include "vs_shader.h"

struct VSTexture
{
    unsigned int id;
    std::string type;
    std::string path;
};

class VSMesh
{
public:
    std::vector<glm::vec3> vertexPositions;
    std::vector<glm::vec3> vertexNormals;
    std::vector<glm::vec2> vertexTexCoords;
    std::vector<glm::vec3> vertexTangents;
    std::vector<glm::vec3> vertexBiTangents;
    std::vector<glm::vec3> vertexColors;
    std::vector<VSTexture> textures;
    std::vector<GLuint> triangleIndices;

    VSMesh(
        std::vector<glm::vec3> vertexPositions,
        std::vector<glm::vec3> vertexNormals,
        std::vector<glm::vec2> vertexTexCoords,
        std::vector<glm::vec3> vertexTangents,
        std::vector<glm::vec3> vertexBiTangents,
        std::vector<glm::vec3> vertexColors,
        std::vector<VSTexture> textures,
        std::vector<GLuint> triangleIndices)
        : vertexPositions(std::move(vertexPositions))
        , vertexNormals(std::move(vertexNormals))
        , vertexTexCoords(std::move(vertexTexCoords))
        , vertexColors(std::move(vertexColors))
        , vertexTangents(std::move(vertexTangents))
        , vertexBiTangents(std::move(vertexBiTangents))
        , textures(std::move(textures))
        , triangleIndices(std::move(triangleIndices))
    {
        setupMesh();
    };

    void draw(VSShader* shader) const
    {
        shader->use();

        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);  // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = textures[i].type;
            if (name == "texture_diffuse")
            {
                number = std::to_string(diffuseNr++);
            }
            else if (name == "texture_specular")
            {
                number = std::to_string(specularNr++);  // transfer unsigned int to stream
            }
            else if (name == "texture_normal")
            {
                number = std::to_string(normalNr++);  // transfer unsigned int to stream
            }
            else if (name == "texture_height")
            {
                number = std::to_string(heightNr++);  // transfer unsigned int to stream
            }

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader->getID(), (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // draw mesh
        glBindVertexArray(vertexArrayObject);
        glDrawElements(GL_TRIANGLES, triangleIndices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    GLuint vertexArrayObject, vertexBuffer, normalBuffer, tangentBuffer, biTangentBuffer,
        colorBuffer, texCordBuffer, indexBuffer;

    void setupMesh()
    {
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        // vertex positions
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertexPositions.size() * sizeof(glm::vec3),
            &vertexPositions[0],
            GL_STATIC_DRAW);

        // vertex normals
        glGenBuffers(1, &normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertexNormals.size() * sizeof(glm::vec3),
            &vertexNormals[0],
            GL_STATIC_DRAW);

        // vertex texCords
        glGenBuffers(1, &texCordBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, texCordBuffer);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertexTexCoords.size() * sizeof(glm::vec2),
            &vertexTexCoords[0],
            GL_STATIC_DRAW);

        // vertex tangents
        glGenBuffers(1, &tangentBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertexTangents.size() * sizeof(glm::vec3),
            &vertexTangents[0],
            GL_STATIC_DRAW);

        // vertex bitangents
        glGenBuffers(1, &biTangentBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, biTangentBuffer);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertexBiTangents.size() * sizeof(glm::vec3),
            &vertexBiTangents[0],
            GL_STATIC_DRAW);

        // vertex colors
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
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

        glBindVertexArray(0);
    };
};
