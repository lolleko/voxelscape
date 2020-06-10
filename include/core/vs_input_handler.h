#pragma once

#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>

class VSInputHandler
{
public:
    VSInputHandler();

    [[nodiscard]] double getYScrollOffset() const;

    // Processes mouse click
    void processMouseButton(GLFWwindow* window, int button, int action, int mods);

    // Processes input received from a mouse input system. Expects the offset value in both the x
    // and y direction.
    void processMouseMovement(
        GLFWwindow* window,
        double xpos,
        double ypos,
        GLboolean constrainPitch = GL_TRUE);

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical
    // wheel-axis
    void processMouseScroll(GLFWwindow* window, double xOffset, double yOffset);

    void processKeyboardInput(GLFWwindow* window, float deltaTime);

    void processFramebufferResize(GLFWwindow* window, int width, int height);

private:
    double yScrollOffset;
};