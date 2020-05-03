#include "vs_world.h"
#include <glm/fwd.hpp>
#include <memory>

VSWorld::VSWorld()
{
    camera = std::make_shared<VSCamera>(glm::vec3(0.0F, 30.0F, 0.0F));
}

const VSBlockData* VSWorld::getBlockData(short ID)
{
    // TODO stub
    return nullptr;
}

void VSWorld::addDrawable(std::shared_ptr<IVSDrawable> drawable, std::shared_ptr<VSShader> shader)
{
    drawables.insert({drawable, shader});
}

void VSWorld::draw(std::shared_ptr<VSWorld> world, std::shared_ptr<VSShader> shader) const
{
    for (const auto& [drawable, drawableShader] : drawables)
    {
        drawable->draw(world, drawableShader);
    }
}

std::shared_ptr<VSCamera> VSWorld::getCamera() const
{
    return camera;
}

glm::vec3 VSWorld::getDirectLightPos() const
{
    return glm::vec3(10000.f, 10000.f, 20000.f);
}

glm::vec3 VSWorld::getDirectLightColor() const
{
    return glm::vec3(1.f, 1.f, 1.f);
}