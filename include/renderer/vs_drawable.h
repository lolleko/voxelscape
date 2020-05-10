#pragma once

#include <memory>

class VSWorld;
class VSShader;

class IVSDrawable
{
public:
    virtual ~IVSDrawable() = 0;

    virtual void draw(VSWorld* world, std::shared_ptr<VSShader> shader) const = 0;
};