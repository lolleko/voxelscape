#include "core/vs_input_handler.h"

VSInputHandler::VSInputHandler()
{
    yScrollOffset = 1.F;
}

double VSInputHandler::getYScrollOffset() const
{
    return yScrollOffset;
}

void VSInputHandler::processKeyboardInput(GLFWwindow* window, float deltaTime)
{
    (void)window;
    (void)deltaTime;
}

void VSInputHandler::processMouseScroll(GLFWwindow *window, double xOffset, double yOffset)
{
    (void) window;
    (void) xOffset;
    yScrollOffset -= yOffset;
}