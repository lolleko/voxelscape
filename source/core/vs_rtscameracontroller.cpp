#include "core/vs_rtscameracontroller.h"
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include "core/vs_camera.h"
#include "world/vs_world.h"
#include "core/vs_input_handler.h"

// TODO: Implement rts camera
VSRTSCameraController::VSRTSCameraController(
    VSCamera* camera,
    VSWorld* world,
    VSInputHandler* inputHandler)
    : VSCameraController(camera, world, inputHandler)
{
}

#include <iostream>
void VSRTSCameraController::updateCamera()
{
    if (!inputHandler)
    {
        return;
    }

    // Handle scroll
    {

    }

    // Handle rotation
    {

    }

    // Handle keyboard movement
    {
        float velocity = movementSpeed * inputHandler->getKeyDeltaTime();
        glm::vec3 position = cam->getPosition();

        VSInputHandler::KEY_FLAGS keyFlags = inputHandler->getKeyFlags();

        if (keyFlags & VSInputHandler::KEY_W)
        {
            glm::vec3 front = cam->getFront();
            front.y = 0.F;
            position += front * velocity;
        }
        if (keyFlags & VSInputHandler::KEY_S)
        {
            glm::vec3 front = cam->getFront();
            front.y = 0.F;
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
    }
}