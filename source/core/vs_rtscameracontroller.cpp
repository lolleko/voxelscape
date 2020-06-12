#include "core/vs_rtscameracontroller.h"
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_projection.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include "core/vs_camera.h"
#include "world/vs_world.h"
#include "world/vs_chunk_manager.h"
#include "core/vs_input_handler.h"

// TODO: Implement rts camera
VSRTSCameraController::VSRTSCameraController(
    VSCamera* camera,
    VSWorld* world,
    VSInputHandler* inputHandler)
    : VSCameraController(camera, world, inputHandler)
{
    cam->setPitchYaw(-45.F, 0.F);
    movementSpeed = 100.F;
    lastYScrollOffset = -45.F;
}

#include <iostream>
void VSRTSCameraController::updateCamera()
{
    if (!inputHandler)
    {
        return;
    }

    // Handle rotation
    {
        // No rotation for RTS camera
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

    bool camHasMoved = false;
    glm::vec3 newPosition = cam->getPosition();
    // Handle keyboard movement
    {
        float velocity = movementSpeed * inputHandler->getKeyDeltaTime();

        VSInputHandler::KEY_FLAGS keyFlags = inputHandler->getKeyFlags();

        if (keyFlags & VSInputHandler::KEY_W)
        {
            glm::vec3 front = cam->getFront();
            front.y = 0.F;
            newPosition += glm::normalize(front) * velocity;
        }
        if (keyFlags & VSInputHandler::KEY_S)
        {
            glm::vec3 front = cam->getFront();
            front.y = 0.F;
            newPosition -= glm::normalize(front) * velocity;
        }
        if (keyFlags & VSInputHandler::KEY_A)
        {
            glm::vec3 right = cam->getRight();
            newPosition -= right * velocity;
        }
        if (keyFlags & VSInputHandler::KEY_D)
        {
            glm::vec3 right = cam->getRight();
            newPosition += right * velocity;
        }
        if (newPosition != cam->getPosition())
        {
            cam->setPosition(newPosition);
            camHasMoved = true;
        }
    }

    // Handle scroll
    {
        float newYOffset = inputHandler->getYScrollOffset();
        float yOffset = lastYScrollOffset - newYOffset;
        if (pitch >= -45.0F && pitch <= 1.0F)
        {
            pitch -= yOffset;
            camHasMoved = true;
        }
        if (pitch <= -45.0F)
        {
            pitch = -45.0F;
            camHasMoved = true;
        }
        if (pitch >= 1.0F)
        {
            pitch = 1.0F;
            camHasMoved = true;
        }
        heightAboveMap = -pitch;
        cam->setPitchYaw(pitch, 0.F);
        lastYScrollOffset = newYOffset;
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

        const auto worldPosNear = glm::unProject(screenPosCamera, tmpView, tmpProj, viewport);
        const auto worldPosFar = glm::unProject(screenPosFar, tmpView, tmpProj, viewport);

        setCameraInWorldCoords(worldPosNear);
        setMouseFarInWorldCoords(worldPosFar);
    }

    // Adapt height to fixpoint
    {
        if (camHasMoved)
        {
            int width = inputHandler->getDisplayWidth();
            int height = inputHandler->getDisplayHeight();
            double xPos = (float)width / 2;
            double ypos = (float)height / 2;
            const auto screenPosCamera = glm::vec3(xPos, double(height) - ypos, 0.F);
            const auto screenPosFar = glm::vec3(xPos, double(height) - ypos, 1.F);

            const auto tmpView = cam->getViewMatrix();
            const auto tmpProj = cam->getProjectionMatrix();
            const glm::vec4 viewport = glm::vec4(0.0F, 0.0F, width, height);

            const auto worldPosNear = glm::unProject(screenPosCamera, tmpView, tmpProj, viewport);
            const auto worldPosFar = glm::unProject(screenPosFar, tmpView, tmpProj, viewport);

            VSChunkManager::VSTraceResult result =
                world->getChunkManager()->lineTrace(worldPosNear, worldPosFar);

            if (result.bHasHit)
            {
                glm::vec3 position = cam->getPosition();
                position.y = result.hitLocation.y + heightAboveMap;
                cam->setPosition(position);
            }
        }
    }
}