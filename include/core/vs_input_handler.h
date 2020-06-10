#pragma once

#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>

class VSInputHandler
{
public:
    VSInputHandler();

    enum KEY_FLAGS
    {
        NONE = 0,
        KEY_A = (1 << 0),
        KEY_S = (1 << 1),
        KEY_D = (1 << 2),
        KEY_W = (1 << 3),
        KEY_Q = (1 << 4),
        KEY_E = (1 << 5),
    };

    [[nodiscard]] double getYScrollOffset() const;

    [[nodiscard]] double getMouseX() const;

    [[nodiscard]] double getMouseY() const;

    [[nodiscard]] bool isLeftMouseClicked() const;

    [[nodiscard]] bool isMiddleMouseClicked() const;

    [[nodiscard]] bool isRightMouseClicked() const;

    [[nodiscard]] KEY_FLAGS getKeyFlags() const;

    [[nodiscard]] float getKeyDeltaTime() const;

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
    float keyDeltaTime = 0.F;

    double yScrollOffset;

    bool leftMouseClicked = false;
    bool middleMouseClicked = false;
    bool rightMouseClicked = false;

    KEY_FLAGS keyFlags = NONE;

    double xMouse = 0;
    double yMouse = 0;
};