#pragma once

#include <glm/common.hpp>
#include <glm/vec3.hpp>

struct Bounds
{
    glm::vec3 min;
    glm::vec3 max;

    [[nodiscard]] glm::vec3 getCenter() const
    {
        return (min + max) / 2.F;
    };

    [[nodiscard]] bool isLocationInside(const glm::vec3& location) const
    {
        return (
            (location.x >= min.x && location.x <= max.x) &&
            (location.y >= min.y && location.y <= max.y) &&
            (location.z >= min.z && location.z <= max.z));
    };

    [[nodiscard]] bool conatainsBounds(const Bounds& bounds) const
    {
        return isLocationInside(bounds.min) && isLocationInside(bounds.max);
    };

    [[nodiscard]] bool isIntersecting(const Bounds& b) const
    {
        return (min.x <= b.max.x && max.x >= b.min.x) && (min.y <= b.max.y && max.y >= b.min.y) &&
               (min.z <= b.max.z && max.z >= b.min.z);
    };

    Bounds operator+(const Bounds& b) const
    {
        return {glm::min(min, b.min), glm::max(max, b.max)};
    };

    Bounds operator+(const glm::vec3& location) const
    {
        return {min + location, max + location};
    };
};
