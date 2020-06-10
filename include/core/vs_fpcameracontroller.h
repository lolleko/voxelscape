#pragma once

#include "core/vs_cameracontroller.h"
#include "world/vs_world.h"

// Forward declarations
class VSCamera;

// Implements first person camera controls
class VSFPCameraController : public VSCameraController
{
public:
    VSFPCameraController(VSCamera* camera, VSWorld* world, VSInputHandler* inputHandler = nullptr);

    void updateCamera() override;

    // Processes mouse click
    void processMouseButton(GLFWwindow* window, int button, int action, int mods) override;

    void processKeyboardInput(GLFWwindow* window, float deltaTime) const override;

    void processFramebufferResize(GLFWwindow* window, int width, int height) override;

private:
    float lastYScrollOffset;
};