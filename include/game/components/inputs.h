// Singleton component, Tag::Input, for holding input states.

#pragma once

#include <entt/entity/fwd.hpp>
#include "game/components/location.h"
#include "game/buildings.h"

enum class InputState
{
    Up,
    JustDown,
    Down,
    JustUp
};

struct Inputs
{
    Location worldMouse;

    // Button/Key states
    InputState leftButtonState;
    InputState rightButtonState;

    entt::entity hoverEntity;

    Buildings selectedBuilding;
};
