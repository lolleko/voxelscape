#include "core/vs_dummycameracontroller.h"
#include "core/vs_cameracontroller.h"

VSDummyCameraController::VSDummyCameraController(
    VSCamera* camera,
    VSWorld* world,
    VSInputHandler* inputHandler)
    : VSCameraController(camera, world, inputHandler)
{
}

void VSDummyCameraController::updateCamera(bool /*handleMouseEvents*/)
{
}
