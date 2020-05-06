#include "vs_model.h"

VSModel::VSModel(std::string const& path, bool gamma)
    : gammaCorrection(gamma)
{
    meshes = std::move(loadModel(path));
}

void VSModel::draw(std::shared_ptr<VSWorld> world, std::shared_ptr<VSShader> shader) const
{
    for (const auto& mesh : meshes)
    {
        mesh.draw(world, shader);
    }
}
