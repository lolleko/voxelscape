#include "core/vs_dummycameracontroller.h"
#include "core/vs_cameracontroller.h"

VSDummyCameraController::VSDummyCameraController(
    VSCamera* camera,
    VSWorld* world,
    VSInputHandler* inputHandler)
    : VSCameraController(camera, world, inputHandler)
{
}

void VSDummyCameraController::updateCamera()
{
}

void VSDummyCameraController::processMouseButton(
    GLFWwindow* /*window*/,
    int /*button*/,
    int /*action*/,
    int /*mods*/)
{
}

void VSDummyCameraController::processKeyboardInput(GLFWwindow* /*window*/, float /*deltaTime*/)
    const
{
}

void VSDummyCameraController::processFramebufferResize(
    GLFWwindow* /*window*/,
    int /*width*/,
    int /*height*/)
{
}
