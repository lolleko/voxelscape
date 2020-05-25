#include "core/vs_dummycameracontroller.h"
#include "core/vs_cameracontroller.h"

VSDummyCameraController::VSDummyCameraController(
    VSCamera* camera,
    VSWorld* world)
    : VSCameraController(camera, world)
{
}

void VSDummyCameraController::processMouseButton(
    GLFWwindow* /*window*/,
    int /*button*/,
    int /*action*/,
    int /*mods*/)
{
}

void VSDummyCameraController::processMouseMovement(
    GLFWwindow* /*window*/,
    double /*xpos*/,
    double /*ypos*/,
    GLboolean /*constrainPitch = GL_TRUE*/)
{
}

void VSDummyCameraController::processMouseScroll(
    GLFWwindow* /*window*/,
    double /*xoffset*/,
    double /*yoffset*/) const
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
