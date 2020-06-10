#pragma once

#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>

class VSInputHandler
{
public:
    VSInputHandler();

    [[nodiscard]] double getYScrollOffset() const;

    [[nodiscard]] double getMouseX() const;

    [[nodiscard]] double getMouseY() const;

    [[nodiscard]] bool isLeftMouseClicked() const;

    [[nodiscard]] bool isMiddleMouseClicked() const;

    [[nodiscard]] bool isRightMouseClicked() const;

    // Processes mouse click
    void processMouseButton(GLFWwindow* window, int button, int action, int mods);

    // Processes input received from a mouse input system. Expects the offset value in both the x
    // and y direction.
    void processMouseMovement(GLFWwindow* window, double xPos, double yPos);

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical
    // wheel-axis
    void processMouseScroll(GLFWwindow* window, double xOffset, double yOffset);

    void processKeyboardInput(GLFWwindow* window, float deltaTime);

    void processFramebufferResize(GLFWwindow* window, int width, int height);

private:
    double yScrollOffset;

    bool leftMouseClicked = false;
    bool middleMouseClicked = false;
    bool rightMouseClicked = false;

    double xMouse = 0;
    double yMouse = 0;
};