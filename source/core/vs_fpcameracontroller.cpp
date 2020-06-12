#include "core/vs_fpcameracontroller.h"
#include <cmath>
#include <glm/ext/matrix_projection.hpp>
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

    // Handle keyboard movement
    {
        float velocity = movementSpeed * inputHandler->getKeyDeltaTime();
        glm::vec3 position = cam->getPosition();

        VSInputHandler::KEY_FLAGS keyFlags = inputHandler->getKeyFlags();

        if (keyFlags & VSInputHandler::KEY_W)
        {
            glm::vec3 front = cam->getFront();
            position += front * velocity;
        }
        if (keyFlags & VSInputHandler::KEY_S)
        {
            glm::vec3 front = cam->getFront();
            position -= front * velocity;
        }
        if (keyFlags & VSInputHandler::KEY_A)
        {
            glm::vec3 right = cam->getRight();
            position -= right * velocity;
        }
        if (keyFlags & VSInputHandler::KEY_D)
        {
            glm::vec3 right = cam->getRight();
            position += right * velocity;
        }
        if (keyFlags & VSInputHandler::KEY_E)
        {
            glm::vec3 up = cam->getUp();
            position += up * velocity;
        }
        if (keyFlags & VSInputHandler::KEY_Q)
        {
            glm::vec3 up = cam->getUp();
            position -= up * velocity;
        }
        cam->setPosition(position);
    }

    // Handle framebufferresize
    {
        if (inputHandler->frameBufferResized())
        {
            float aspectRatio = inputHandler->getAspectRatio();
            cam->setAspectRatio(aspectRatio);
            inputHandler->frameBufferResizeHandled();
        }
    }

    // Calculate mouse coords in world space
    {
        double xPos = inputHandler->getMouseX();
        double ypos = inputHandler->getMouseY();
        int width = inputHandler->getDisplayWidth();
        int height = inputHandler->getDisplayHeight();
        const auto screenPosCamera = glm::vec3(xPos, double(height) - ypos, 0.F);
        const auto screenPosFar = glm::vec3(xPos, double(height) - ypos, 1.F);

        const auto tmpView = cam->getViewMatrix();
        const auto tmpProj = cam->getProjectionMatrix();
        const glm::vec4 viewport = glm::vec4(0.0F, 0.0F, width, height);

        auto worldPosNear = glm::unProject(screenPosCamera, tmpView, tmpProj, viewport);
        auto worldPosFar = glm::unProject(screenPosFar, tmpView, tmpProj, viewport);

        setCameraInWorldCoords(worldPosNear);
        setMouseFarInWorldCoords(worldPosFar);
    }
}