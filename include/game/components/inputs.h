// Singleton component, Tag::Input, for holding input states.

#pragma once

#include <entt/entity/fwd.hpp>
#include "game/components/location.h"
#include "game/components/unique.h"
#include "world/vs_chunk_manager.h"
#include "core/vs_input_handler.h"

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
    bool anyWindowHovered;

    // Button/Key states
    InputState leftButtonState;
    InputState rightButtonState;

    VSInputHandler::KEY_FLAGS Up;
    VSInputHandler::KEY_FLAGS JustDown;
    VSInputHandler::KEY_FLAGS Down;
    VSInputHandler::KEY_FLAGS JustUp;

    entt::entity hoverEntity;

    Unique selectedBuilding;

    bool bShouldResetSelection;

    bool bIsBuildingPreviewInitialized;

    bool bIsBuildingPreviewConstructed;
};
