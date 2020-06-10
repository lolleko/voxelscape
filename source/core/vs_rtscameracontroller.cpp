#include "core/vs_rtscameracontroller.h"
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include "core/vs_camera.h"
#include "world/vs_world.h"

// TODO: Implement rts camera
VSRTSCameraController::VSRTSCameraController(VSCamera* camera, VSWorld* world, VSInputHandler* inputHandler)
    : VSCameraController(camera, world, inputHandler)
{
}

void VSRTSCameraController::processMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    (void) mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos;
        double ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        lastX = xpos;
        lastY = ypos;
    }
}