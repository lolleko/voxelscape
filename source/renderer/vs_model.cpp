#include "renderer/vs_model.h"
#include "world/vs_world.h"
#include "core/vs_camera.h"

VSModel::VSModel(std::string const& path, bool gamma)
    : gammaCorrection(gamma)
{
    meshes = std::move(loadModel(path));
}

VSModel::~VSModel() {

}

void VSModel::draw(VSWorld* world, std::shared_ptr<VSShader> shader) const
{
    const auto model = glm::scale(glm::mat4(1.f), {3.f, 3.f, 3.f});
    shader->uniforms()
        .setVec3("lightPos", world->getDirectLightPos())
        .setVec3("lightColor", world->getDirectLightColor())
        .setVec3("viewPos", world->getCamera()->getPosition())
        .setMat4("model", model)
        .setMat4("MVP", world->getCamera()->getMVPMatrixFast(model));

    for (const auto& mesh : meshes)
    {
        mesh.draw(world, shader);
    }
}
