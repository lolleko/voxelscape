#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entt.hpp>
#include <glm/geometric.hpp>

#include "core/vs_app.h"
#include "core/vs_input_handler.h"
#include "core/vs_debug_draw.h"
#include "core/vs_cameracontroller.h"
#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"
#include "game/components/inputs.h"
#include "world/vs_world.h"
#include "world/vs_chunk_manager.h"

#include "game/components/world_context.h"

InputState calculateMouseStateBasedOnPrevious(InputState previous, bool bIsClicked)
{
    switch (previous)
    {
        case InputState::Up:
        case InputState::JustUp:
            return bIsClicked ? InputState::JustDown : InputState::Up;
        case InputState::JustDown:
        case InputState::Down:
            return bIsClicked ? InputState::Down : InputState::JustUp;
        default:
            return InputState::Up;
    }
}

void updateInputSystem(entt::registry& registry)
{
    const auto* inputHandler = VSApp::getInstance()->getInputHandler();
    auto* UI = VSApp::getInstance()->getUI();

    // at startup we wont have a previousInput -> create one
    const auto* previousInputsPtr = registry.try_ctx<Inputs>();
    if (previousInputsPtr == nullptr)
    {
        registry.set<Inputs>(
            VSChunkManager::VSTraceResult{},
            UI->getState()->anyWindowHovered,
            inputHandler->isLeftMouseClicked() ? InputState::Down : InputState::Up,
            inputHandler->isRightMouseClicked() ? InputState::Down : InputState::Up,
            entt::null,
            "null");
    }

    const auto& previousInputs = registry.ctx<Inputs>();

    const auto* world = registry.ctx<WorldContext>().world;

    glm::vec3 worldPosNear = world->getCameraController()->getCameraInWorldCoords();
    glm::vec3 worldPosFar = world->getCameraController()->getMouseFarInWorldCoords();

    const auto newMouseTrace = world->getChunkManager()->lineTrace(worldPosNear, worldPosFar);

    if (newMouseTrace.bHasHit)
    {
        // world->getDebugDraw()->drawSphere(newMouseTrace.hitLocation, 0.25F, {255, 0, 0});
    }

    const auto newLeftButtonState = calculateMouseStateBasedOnPrevious(
        previousInputs.leftButtonState, inputHandler->isLeftMouseClicked());
    const auto newRightButtonState = calculateMouseStateBasedOnPrevious(
        previousInputs.rightButtonState, inputHandler->isRightMouseClicked());

    const auto newSelectedBuilding = UI->getState()->selectedBuilding;

    const auto isAnyWindowHovered = UI->getState()->anyWindowHovered;

    registry.set<Inputs>(
        newMouseTrace,
        isAnyWindowHovered,
        newLeftButtonState,
        newRightButtonState,
        previousInputs.hoverEntity,
        newSelectedBuilding);
}
