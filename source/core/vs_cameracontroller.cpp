#include <glm/fwd.hpp>
#include <iostream>

#include "core/vs_cameracontroller.h"
#include "core/vs_camera.h"

VSCameraController::VSCameraController(VSCamera* camera, VSWorld* world)
    : mouseSensitivity(SENSITIVITY)
    , movementSpeed(SPEED)
{
    cam = camera;
    this->world = world;
}