#pragma once

#include <glm/fwd.hpp>

class IVSTransformable
{
public:
    virtual glm::mat4 getModelMatrix() const = 0;

    virtual void setModelMatrix(const glm::mat4& mat) = 0;
};