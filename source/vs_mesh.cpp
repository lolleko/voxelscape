#include "vs_mesh.h"

VSMesh::VSMesh(std::unique_ptr<VSVertexContext> vertexContext, std::vector<VSTexture> textures)
    : vertexContext(std::move(vertexContext))
    , textures(std::move(textures)){};

void VSMesh::draw(std::shared_ptr<VSWorld> world, std::shared_ptr<VSShader> shader) const
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
    glBindVertexArray(vertexContext->vertexArrayObject);
    glDrawElements(GL_TRIANGLES, vertexContext->triangleCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}
