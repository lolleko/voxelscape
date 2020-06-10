#include "core/vs_input_handler.h"

VSInputHandler::VSInputHandler()
{

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