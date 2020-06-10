#include <glm/fwd.hpp>

#include "core/vs_cameracontroller.h"
#include "core/vs_camera.h"

VSCameraController::VSCameraController(
    VSCamera* camera,
    VSWorld* world,
    VSInputHandler* inputHandler)
    : mouseSensitivity(SENSITIVITY)
    , movementSpeed(SPEED)
{
    this->cam = camera;
    this->world = world;
    this->inputHandler = inputHandler;
}

void VSCameraController::setEditorBlockID(int blockID)
{
    editorBlockID = blockID;
}

void VSCameraController::setMouseInWorldCoords(glm::vec3 coords)
{
    mouseInWorldCoords = coords;
}

void VSCameraController::setMouseNormalInWorldCoords(glm::vec3 normal)
{
    mouseNormalInWorldCoords = normal;
}

glm::vec3 VSCameraController::getMouseInWorldCoords() const
{
    return mouseInWorldCoords;
}

glm::vec3 VSCameraController::getMouseNormalInWorldCoords() const
{
    return mouseNormalInWorldCoords;
}

void VSCameraController::setInputHandler(VSInputHandler* inputHandler)
{
    this->inputHandler = inputHandler;
}