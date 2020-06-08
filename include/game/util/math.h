#include "game/components/bounds.h"

bool isLocationInBounds(const glm::vec3& location, Bounds bounds)
{
    return (
        (location.x >= bounds.min.x && location.x <= bounds.max.x) &&
        (location.y >= bounds.min.y && location.y <= bounds.max.y) &&
        (location.z >= bounds.min.z && location.z <= bounds.max.z));
}