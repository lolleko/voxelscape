#pragma once

#include <memory>

class VSWorld;
class VSShader;

class IVSDrawable
{
public:
    virtual void draw(std::shared_ptr<VSWorld> world, std::shared_ptr<VSShader> shader) const = 0;
};