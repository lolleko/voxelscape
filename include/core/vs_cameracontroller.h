#pragma once

#include <glad/glad.h>  // Initialize with gladLoadGL()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include <memory>
#include "world/vs_world.h"

// Forward declarations
class VSCamera;
struct GLFWwindow;

const float SPEED = 100.F;
const float SENSITIVITY = 0.1F;

// Implements Observer pattern with VSCameraController being the subject and VSCamera being the
// Obser
class VSCameraController
{
public:
    VSCameraController(VSCamera* camera, VSWorld* world);
    virtual ~VSCameraController() = default;

    // Processes mouse click
    virtual void processMouseButton(GLFWwindow* window, int button, int action, int mods) = 0;

    // Processes input received from a mouse input system. Expects the offset value in both the x
    // and y direction.
    virtual void processMouseMovement(
        GLFWwindow* window,
        double xpos,
        double ypos,
        GLboolean constrainPitch = GL_TRUE) = 0;

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical
    // wheel-axis
    virtual void processMouseScroll(GLFWwindow* window, double xoffset, double yoffset) const = 0;

    virtual void processKeyboardInput(GLFWwindow* window, float deltaTime) const = 0;

    virtual void processFramebufferResize(GLFWwindow* window, int width, int height) = 0;

protected:
    VSCamera* cam;
    VSWorld* world;

    bool firstMouse = true;
    float lastX = 0.F;
    float lastY = 0.F;

    float mouseSensitivity;
    float movementSpeed;
};