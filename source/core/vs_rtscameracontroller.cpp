#include "core/vs_rtscameracontroller.h"
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include "core/vs_camera.h"
#include "world/vs_world.h"

// TODO: Implement rts camera
VSRTSCameraController::VSRTSCameraController(VSCamera* camera, VSWorld* world, VSInputHandler* inputHandler)
    : VSCameraController(camera, world, inputHandler)
{
}

void VSRTSCameraController::processMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    (void) mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos;
        double ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        lastX = xpos;
        lastY = ypos;
    }
}

void VSRTSCameraController::processMouseMovement(
    GLFWwindow* window,
    double xpos,
    double ypos,
    GLboolean constrainPitch)
{
    // Only move camera if left mouse is pressed
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state != GLFW_PRESS)
    {
        return;
    }

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    float yaw = cam->getYaw();
    float pitch = cam->getPitch();
    yaw += xoffset;
    pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch == GL_TRUE)
    {
        if (pitch > 89.0F)
        {
            pitch = 89.0F;
        }
        if (pitch < -89.0F)
        {
            pitch = -89.0F;
        }
    }
    cam->setPitchYaw(pitch, yaw);
}

void VSRTSCameraController::processKeyboardInput(GLFWwindow* window, float deltaTime) const
{
    float velocity = movementSpeed * deltaTime;
    glm::vec3 position = cam->getPosition();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        glm::vec3 front = cam->getFront();
        position += front * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        glm::vec3 front = cam->getFront();
        position -= front * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        glm::vec3 right = cam->getRight();
        position -= right * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        glm::vec3 right = cam->getRight();
        position += right * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        glm::vec3 up = cam->getUp();
        position += up * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        glm::vec3 up = cam->getUp();
        position -= up * velocity;
    }
    cam->setPosition(position);
}

void VSRTSCameraController::processFramebufferResize(GLFWwindow* window, int width, int height)
{
    (void) window;

    float aspectRatio = (float)width / height;
    cam->setAspectRatio(aspectRatio);
}