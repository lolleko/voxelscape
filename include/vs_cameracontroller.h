#pragma once

#include <glad/glad.h>  // Initialize with gladLoadGL()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include <memory>

// Forward declarations
class VSCamera;
class GLFWwindow;

const float SPEED = 100.F;
const float SENSITIVITY = 0.1F;

// Implements Observer pattern with VSCameraController being the subject and VSCamera being the
// Obser
class VSCameraController
{
public:
    VSCameraController(const std::shared_ptr<VSCamera>& camera);

    void processMouseButton(GLFWwindow* window, int button, int action, int mods);

    // Processes input received from a mouse input system. Expects the offset value in both the x
    // and y direction.
    void
    processMouseMovement(GLFWwindow* window, double xpos, double ypos, GLboolean constrainPitch = GL_TRUE);

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical
    // wheel-axis
    void processMouseScroll(GLFWwindow* window, double xoffset, double yoffset) const;

    void processKeyboardInput(GLFWwindow* window, float deltaTime) const;

private:
    std::shared_ptr<VSCamera> cam;

    bool firstMouse = true;
    float lastX = 0.F;
    float lastY = 0.F;

    float mouseSensitivity;
    float movementSpeed;
};