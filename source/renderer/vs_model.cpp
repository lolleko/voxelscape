#include "renderer/vs_model.h"

#include <glm/ext/matrix_transform.hpp>

#include "world/vs_world.h"
#include "core/vs_camera.h"

VSModel::VSModel(std::string const& path)
{
    meshes = loadModel(path);
}

void VSModel::draw(VSWorld* world)
{
    const auto model = glm::scale(glm::mat4(1.f), {3.f, 3.f, 3.f});
    modelShader.uniforms()
        .setVec3("lightDir", world->getDirectLightDir())
        .setVec3("lightColor", world->getDirectLightColor())
        .setVec3("viewPos", world->getCamera()->getPosition())
        .setMat4("model", model)
        .setMat4("MVP", world->getCamera()->getMVPMatrixFast(model));

    for (const auto& mesh : meshes)
    {
        mesh.draw(modelShader);
    }
}
