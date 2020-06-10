#pragma once

#include "core/vs_cameracontroller.h"
#include "world/vs_world.h"

// Forward declarations
class VSCamera;

class VSRTSCameraController : public VSCameraController
{
public:
    VSRTSCameraController(VSCamera* camera, VSWorld* world, VSInputHandler* inputHandler = nullptr);

    // Processes mouse click
    void processMouseButton(GLFWwindow* window, int button, int action, int mods) override;

    void processKeyboardInput(GLFWwindow* window, float deltaTime) const override;

    void processFramebufferResize(GLFWwindow* window, int width, int height) override;
};