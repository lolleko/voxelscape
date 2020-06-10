#include "core/vs_input_handler.h"
#include <GLFW/glfw3.h>

VSInputHandler::VSInputHandler()
{
    yScrollOffset = 1.F;
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

bool VSInputHandler::isLeftMouseClicked() const
{
    return leftMouseClicked;
}

bool VSInputHandler::isMiddleMouseClicked() const
{
    return middleMouseClicked;
}

bool VSInputHandler::isRightMouseClicked() const
{
    return rightMouseClicked;
}

void VSInputHandler::processKeyboardInput(GLFWwindow* window, float deltaTime)
{
    (void)window;
    (void)deltaTime;
}

void VSInputHandler::processMouseScroll(GLFWwindow* window, double xOffset, double yOffset)
{
    (void)window;
    (void)xOffset;
    yScrollOffset -= yOffset;
}

void VSInputHandler::processMouseMovement(GLFWwindow* window, double xPos, double yPos)
{
    (void)window;
    xMouse = xPos;
    yMouse = yPos;
}

void VSInputHandler::processMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    (void)window;
    (void)mods;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        leftMouseClicked = true;
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        middleMouseClicked = true;
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        rightMouseClicked = true;
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        leftMouseClicked = false;
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        middleMouseClicked = false;
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        rightMouseClicked = false;
        return;
    }
}