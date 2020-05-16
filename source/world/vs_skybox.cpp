#include "world/vs_skybox.h"

#include "renderer/vs_textureloader.h"
#include "world/vs_world.h"
#include "core/vs_camera.h"

// Based on https://learnopengl.com/Advanced-OpenGL/Cubemaps
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

void VSSkybox::draw(VSWorld* world) const
{
    glDepthFunc(GL_LEQUAL);

    skyboxShader.uniforms()
        .setMat4("view", glm::mat4(glm::mat3(world->getCamera()->getViewMatrix())))
        .setMat4("projection", world->getCamera()->getProjectionMatrix())
        .setVec3("viewPos", world->getCamera()->getPosition())
        .setMat4("model", getModelMatrix())
        .setMat4("MVP", world->getCamera()->getMVPMatrixFast(getModelMatrix()));

    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);  // set depth function back to defaults
}

glm::mat4 VSSkybox::getModelMatrix() const
{
    return glm::mat4(1.f);
}
void VSSkybox::setModelMatrix(const glm::mat4& mat){
    (void) mat;
    // TODO
};