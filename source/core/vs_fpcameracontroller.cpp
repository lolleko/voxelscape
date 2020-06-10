#include "core/vs_fpcameracontroller.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/matrix.hpp>
#include "core/vs_camera.h"
#include "core/vs_debug_draw.h"
#include "core/vs_input_handler.h"
#include "world/vs_world.h"
#include "world/vs_chunk_manager.h"

VSFPCameraController::VSFPCameraController(
    VSCamera* camera,
    VSWorld* world,
    VSInputHandler* inputHandler)
    : VSCameraController(camera, world, inputHandler)
{
    lastYScrollOffset = 0.F;
}

void VSFPCameraController::updateCamera()
{
    if (!inputHandler)
    {
        return;
    }

    // Handle scroll
    {
        float newYOffset = inputHandler->getYScrollOffset();
        float yOffset = lastYScrollOffset - newYOffset;
        float zoom = cam->getZoom();
        if (zoom >= 1.0F && zoom <= 45.0F)
        {
            zoom -= yOffset;
        }
        if (zoom <= 1.0F)
        {
            zoom = 1.0F;
        }
        if (zoom >= 45.0F)
        {
            zoom = 45.0F;
        }
        cam->setZoom(zoom);
        lastYScrollOffset = newYOffset;
    }

    // Handle rotation
    {
        float xPos = inputHandler->getMouseX();
        float yPos = inputHandler->getMouseY();
        if (inputHandler->isLeftMouseClicked())
        {
            if (firstMouse)
            {
                lastX = xPos;
                lastY = yPos;
                firstMouse = false;
            }
            else
            {
                float xoffset = xPos - lastX;
                float yoffset = lastY - yPos;  // reversed since y-coordinates go from bottom to top

                lastX = xPos;
                lastY = yPos;

                xoffset *= mouseSensitivity;
                yoffset *= mouseSensitivity;

                float yaw = cam->getYaw();
                float pitch = cam->getPitch();
                yaw += xoffset;
                pitch += yoffset;

                // Make sure that when pitch is out of bounds, screen doesn't get flipped
                if (/*constrainPitch == */ GL_TRUE)
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
        }
        else
        {
            lastX = xPos;
            lastY = yPos;
        }
    }
}

void VSFPCameraController::processMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos;
        double ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        lastX = xpos;
        lastY = ypos;
    }

    // Set block on cursor position
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        std::cout << mouseInWorldCoords.x << ", " << mouseInWorldCoords.y << ", "
                  << mouseInWorldCoords.z << std::endl;
        // Check if block is placed in bounds
        if (!world->getChunkManager()->isLocationInBounds(mouseInWorldCoords))
        {
            // do nothing
            return;
        }
        world->getChunkManager()->setBlock(mouseInWorldCoords, editorBlockID);
    }

    // Delete block on cursor position
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // Check if block is placed in bounds
        if (!world->getChunkManager()->isLocationInBounds(mouseInWorldCoords))
        {
            // do nothing
            return;
        }
        world->getChunkManager()->setBlock(mouseInWorldCoords, 0);
    }
}

void VSFPCameraController::processMouseMovement(
    GLFWwindow* window,
    double xpos,
    double ypos,
    GLboolean constrainPitch)
{
    // Only move camera if left mouse is pressed
    // (void)window;
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

void VSFPCameraController::processKeyboardInput(GLFWwindow* window, float deltaTime) const
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

void VSFPCameraController::processFramebufferResize(GLFWwindow* window, int width, int height)
{
    (void)window;

    float aspectRatio = (float)width / height;
    cam->setAspectRatio(aspectRatio);
}