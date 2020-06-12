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