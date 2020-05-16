#include <glm/fwd.hpp>
#include <iostream>

#include "core/vs_cameracontroller.h"
#include "core/vs_camera.h"

VSCameraController::VSCameraController(VSCamera* camera)
    : mouseSensitivity(SENSITIVITY)
    , movementSpeed(SPEED)
{
    cam = camera;
}