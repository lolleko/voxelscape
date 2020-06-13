// Singleton component, Tag::Input, for holding input states.

#pragma once

#include <entt/entity/fwd.hpp>
#include "game/components/location.h"
#include "game/buildings.h"
#include "world/vs_chunk_manager.h"

enum class InputState
{
    Up,
    JustDown,
    Down,
    JustUp
};

struct Inputs
{
    VSChunkManager::VSTraceResult mouseTrace;

    // Button/Key states
    InputState leftButtonState;
    InputState rightButtonState;

    entt::entity hoverEntity;

    Buildings selectedBuilding;
};
