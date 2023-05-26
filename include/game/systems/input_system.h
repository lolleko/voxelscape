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

    // at startup we wont have a previousInput -> create one
    const auto previousInputs = registry.ctx().get<Inputs>();

    const auto* world = registry.ctx().get<WorldContext>().world;

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
    const auto newMiddleButtonState = calculateMouseStateBasedOnPrevious(
        previousInputs.middleButtonState, inputHandler->isMiddleMouseClicked());

    auto& inputs = registry.ctx().get<Inputs>();

    inputs.mouseTrace = newMouseTrace;
    inputs.leftButtonState = newLeftButtonState;
    inputs.rightButtonState = newRightButtonState;
    inputs.middleButtonState = newMiddleButtonState;
    inputs.Up = VSInputHandler::KEY_FLAGS(~inputHandler->getKeyFlags());
    inputs.JustUp = VSInputHandler::KEY_FLAGS(previousInputs.Up & inputHandler->getKeyFlags());
    inputs.Down = VSInputHandler::KEY_FLAGS(inputHandler->getKeyFlags());
    inputs.JustDown = VSInputHandler::KEY_FLAGS(previousInputs.Down & (~inputHandler->getKeyFlags()));
    inputs.hoverEntity = previousInputs.hoverEntity;
}
