#pragma once

#include <memory>

// Forward declarations
class VSCamera;
class GLFWwindow;

// Implements Observer pattern with VSCameraController being the subject and VSCamera being the
// Obser
class VSCameraController
{
public:
    VSCameraController(const std::shared_ptr<VSCamera>& camera);

    void processMouseButton(GLFWwindow* window, int button, int action, int mods);
    void processMouseMovement(GLFWwindow* window, double xpos, double ypos);
    void processMouseScroll(GLFWwindow* window, double xoffset, double yoffset) const;
    void processKeyboardInput(GLFWwindow* window, float deltaTime) const;

private:
    std::shared_ptr<VSCamera> cam;

    bool firstMouse = true;
    float lastX = 0.F;
    float lastY = 0.F;
};