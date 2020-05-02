#include "vs_skybox.h"

#include "vs_textureloader.h"

VSSkybox::VSSkybox()
{
    // skybox VAO
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
    // load textures
    cubemapTexture = loadSkyboxCubemap();
}

void VSSkybox::draw(std::shared_ptr<VSShader> shader) const
{
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal
                             // to depth buffer's content
    shader->use();
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);  // set depth function back to defaults
}