#include "core/vs_input_handler.h"
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include "core/vs_log.h"
#include "game/components/inputs.h"
#include "game/components/location.h"

VSInputHandler::VSInputHandler(int displayWidth, int displayHeight)
{
    this->yScrollOffset = 1.F;
    this->displayWidth = displayWidth;
    this->displayHeight = displayHeight;
}

Inputs VSInputHandler::getInputState() const
{
    Inputs inputs;

    if (!leftClickHandled)
    {
        inputs.leftButtonState = InputState::JustDown;
    }
    else if (leftMouseClicked)
    {
        inputs.leftButtonState = InputState::Down;
    }

    if (!rightClickHandled)
    {
        inputs.rightButtonState = InputState::JustDown;
    }
    else if (rightMouseClicked)
    {
        inputs.rightButtonState = InputState::Down;
    }

    // Put mouseInWorldPos here
    // inputs.worldMouse = mouseInWorldPos;

    return inputs;
}

double VSInputHandler::getYScrollOffset() const
{
    return yScrollOffset;
}

double VSInputHandler::getMouseX() const
{
    return xMouse;
}

double VSInputHandler::getMouseY() const
{
    return yMouse;
}

int VSInputHandler::getDisplayWidth() const
{
    return displayWidth;
}

int VSInputHandler::getDisplayHeight() const
{
    return displayHeight;
}

bool VSInputHandler::isLeftMouseClicked() const
{
    return leftMouseClicked;
}

bool VSInputHandler::isLeftClickHandled() const
{
    return leftClickHandled;
}

bool VSInputHandler::isMiddleMouseClicked() const
{
    return middleMouseClicked;
}

bool VSInputHandler::isMiddleClickHandled() const
{
    return middleClickHandled;
}

bool VSInputHandler::isRightMouseClicked() const
{
    return rightMouseClicked;
}

bool VSInputHandler::isRightClickHandled() const
{
    return rightClickHandled;
}

VSInputHandler::KEY_FLAGS VSInputHandler::getKeyFlags() const
{
    return keyFlags;
}

float VSInputHandler::getKeyDeltaTime() const
{
    return keyDeltaTime;
}

float VSInputHandler::getAspectRatio() const
{
    return aspectRatio;
}

bool VSInputHandler::frameBufferResized() const
{
    return aspectRatioChanged;
}

void VSInputHandler::frameBufferResizeHandled()
{
    if (aspectRatioChanged == false)
    {
        // Not good, framebufffer resize should only be handled once
        VSLog::Log(
            VSLog::Category::Core,
            VSLog::Level::warn,
            "Framebuffer resize got handled more than once");
    }
    aspectRatioChanged = false;
}

glm::vec3 VSInputHandler::getMouseInWorldPos() const
{
    return mouseInWorldPos;
}

void VSInputHandler::setMouseInWorldPos(glm::vec3 worldPos)
{
    mouseInWorldPos = worldPos;
}

void VSInputHandler::handleRightClick()
{
    rightClickHandled = true;
}

void VSInputHandler::handleMiddleClick()
{
    middleClickHandled = true;
}

void VSInputHandler::handleLeftClick()
{
    leftClickHandled = true;
}

void VSInputHandler::processKeyboardInput(GLFWwindow* window, float deltaTime)
{
    keyDeltaTime = deltaTime;

    // Press
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        keyFlags = KEY_FLAGS(keyFlags | KEY_W);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        keyFlags = KEY_FLAGS(keyFlags | KEY_S);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        keyFlags = KEY_FLAGS(keyFlags | KEY_A);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        keyFlags = KEY_FLAGS(keyFlags | KEY_D);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        keyFlags = KEY_FLAGS(keyFlags | KEY_E);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        keyFlags = KEY_FLAGS(keyFlags | KEY_Q);
    }

    // Release
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
    {
        keyFlags = KEY_FLAGS(keyFlags & ~KEY_W);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
    {
        keyFlags = KEY_FLAGS(keyFlags & ~KEY_S);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE)
    {
        keyFlags = KEY_FLAGS(keyFlags & ~KEY_A);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
    {
        keyFlags = KEY_FLAGS(keyFlags & ~KEY_D);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE)
    {
        keyFlags = KEY_FLAGS(keyFlags & ~KEY_E);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
    {
        keyFlags = KEY_FLAGS(keyFlags & ~KEY_Q);
    }
}

void VSInputHandler::processMouseScroll(GLFWwindow* /*window*/, double /*xOffset*/, double yOffset)
{
    yScrollOffset -= yOffset;
}

void VSInputHandler::processMouseMovement(GLFWwindow* /*window*/, double xPos, double yPos)
{
    xMouse = xPos;
    yMouse = yPos;
}

void VSInputHandler::processMouseButton(
    GLFWwindow* /*window*/,
    int button,
    int action,
    int /*mods*/)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // first time, should be handled
        leftClickHandled = false;
        leftMouseClicked = true;
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // first time, should be handled
        middleClickHandled = false;
        middleMouseClicked = true;
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // first time, should be handled
        rightClickHandled = false;
        rightMouseClicked = true;
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // released
        leftMouseClicked = false;
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // released
        middleMouseClicked = false;
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        // released
        rightMouseClicked = false;
        return;
    }
}

void VSInputHandler::processFramebufferResize(GLFWwindow* /*window*/, int width, int height)
{
    displayWidth = width;
    displayHeight = height;
    aspectRatio = (float)width / height;
    aspectRatioChanged = true;
}