#pragma once

#include "core/vs_cameracontroller.h"
#include "game/components/bounds.h"
#include "world/vs_chunk_manager.h"
#include "world/vs_world.h"

struct WorldContext
{
    VSWorld* world;
    float deltaSeconds;
    float worldAge;
    Bounds bounds;
};