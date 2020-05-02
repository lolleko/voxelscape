#pragma once

#include <memory>
#include "vs_shader.h"

class IVSDrawable
{
public:
    virtual void draw(std::shared_ptr<VSShader> shader) const = 0;
};