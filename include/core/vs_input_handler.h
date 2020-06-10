#pragma once

#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>

#include "game/components/inputs.h"

class VSInputHandler
{
public:
    VSInputHandler(int displayWidth, int displayHeight);

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

    [[nodiscard]] Inputs getInputState() const;

    [[nodiscard]] double getYScrollOffset() const;

    [[nodiscard]] double getMouseX() const;

    [[nodiscard]] double getMouseY() const;

    [[nodiscard]] bool isLeftMouseClicked() const;

    [[nodiscard]] bool isLeftClickHandled() const;

    [[nodiscard]] bool isMiddleMouseClicked() const;

    [[nodiscard]] bool isMiddleClickHandled() const;

    [[nodiscard]] bool isRightMouseClicked() const;

    [[nodiscard]] bool isRightClickHandled() const;

    [[nodiscard]] KEY_FLAGS getKeyFlags() const;

    [[nodiscard]] float getKeyDeltaTime() const;

    [[nodiscard]] float getAspectRatio() const;

    [[nodiscard]] bool frameBufferResized() const;

    [[nodiscard]] int getDisplayWidth() const;

    [[nodiscard]] int getDisplayHeight() const;

    [[nodiscard]] glm::vec3 getMouseInWorldPos() const;

    void setMouseInWorldPos(glm::vec3 worldPos);

    void handleLeftClick();

    void handleRightClick();

    void handleMiddleClick();

    void frameBufferResizeHandled();

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

    // This data is also available in cameracontroller, but will be set here to pass to entt inputs
    glm::vec3 mouseInWorldPos = {0.F, 0.F, 0.F};

    // Not really user input but corresponds to resizing the window and currently this data is not kept anywhere else
    int displayWidth = 0;
    int displayHeight = 0;

    float aspectRatio = 0.F;
    bool aspectRatioChanged = false;

    double yScrollOffset;

    bool leftMouseClicked = false;
    bool leftClickHandled = true;

    bool middleMouseClicked = false;
    bool middleClickHandled = true;

    bool rightMouseClicked = false;
    bool rightClickHandled = true;

    KEY_FLAGS keyFlags = NONE;

    double xMouse = 0;
    double yMouse = 0;
};