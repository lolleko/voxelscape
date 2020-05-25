#pragma once

#include "core/vs_cameracontroller.h"

// Forward declarations
class VSCamera;

// Dummy camera controller with no functionality. Supposed to be used for static menu backgroud world.
class VSDummyCameraController : public VSCameraController
{
public:
    VSDummyCameraController(VSCamera* camera = nullptr, VSWorld* world = nullptr);

    // Processes mouse click
    void processMouseButton(GLFWwindow* window, int button, int action, int mods) override;

    // Processes input received from a mouse input system. Expects the offset value in both the x
    // and y direction.
    void processMouseMovement(
        GLFWwindow* window,
        double xpos,
        double ypos,
        GLboolean constrainPitch = GL_TRUE) override;

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical
    // wheel-axis
    void processMouseScroll(GLFWwindow* window, double xoffset, double yoffset) const override;

    void processKeyboardInput(GLFWwindow* window, float deltaTime) const override;

    void processFramebufferResize(GLFWwindow* window, int width, int height) override;
};