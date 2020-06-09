#include <glm/fwd.hpp>
#include <iostream>

#include "core/vs_cameracontroller.h"
#include "core/vs_camera.h"

VSCameraController::VSCameraController(VSCamera* camera, VSWorld* world)
    : mouseSensitivity(SENSITIVITY)
    , movementSpeed(SPEED)
{
    this->cam = camera;
    this->world = world;
}

void VSCameraController::setEditorBlockID(int blockID)
{
    editorBlockID = blockID;
}

void VSCameraController::setMouseInWorldCoords(glm::vec3 coords)
{
    mouseInWorldCoords = coords;
}

glm::vec3 VSCameraController::getMouseInWorldCoords() const
{
    return mouseInWorldCoords;
}