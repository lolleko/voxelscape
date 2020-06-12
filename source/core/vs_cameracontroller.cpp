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

void VSCameraController::setCameraInWorldCoords(glm::vec3 coords)
{
    cameraInWorldCoords = coords;
}

void VSCameraController::setMouseFarInWorldCoords(glm::vec3 far)
{
    mouseFarInWorldCoords = far;
}

glm::vec3 VSCameraController::getCameraInWorldCoords() const
{
    return cameraInWorldCoords;
}

glm::vec3 VSCameraController::getMouseFarInWorldCoords() const
{
    return mouseFarInWorldCoords;
}

void VSCameraController::setInputHandler(VSInputHandler* inputHandler)
{
    this->inputHandler = inputHandler;
}