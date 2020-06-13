#include "core/vs_rtscameracontroller.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/ext/matrix_projection.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/compatibility.hpp>
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
    cam->setPitchYaw(pitch, yaw);
    movementSpeed = 100.F;
    // lastYScrollOffset = -45.F;
}

#include <iostream>
void VSRTSCameraController::updateCamera()
{
    if (!inputHandler)
    {
        return;
    }

    // Calculate old sphere coordinates
    glm::vec3 oldSphere;
    float radius = 50.F;
    {
        float pitchRad = pitch * (M_PI / 180.F);
        float yawRad = -(yaw - 90.F) * (M_PI / 180.F);
        // Y is height axis
        oldSphere.z = radius * std::sin(pitchRad) * std::cos(yawRad);
        oldSphere.x = radius * std::sin(pitchRad) * std::sin(yawRad);
        oldSphere.y = radius * std::cos(pitchRad);
    }

    // Handle rotation
    {
        // No rotation for RTS camera
        yaw += 0.1F;
    }

    // Calculate new sphere coordinates
    glm::vec3 newSphere;
    {
        float pitchRad = pitch * (M_PI / 180.F);
        float yawRad = -(yaw - 90.F) * (M_PI / 180.F);
        // Y is height axis
        newSphere.z = radius * std::sin(pitchRad) * std::cos(yawRad);
        newSphere.x = radius * std::sin(pitchRad) * std::sin(yawRad);
        newSphere.y = radius * std::cos(pitchRad);
    }

    std::cout << (newSphere - oldSphere).x << ", " << (newSphere - oldSphere).y << ", "
              << (newSphere - oldSphere).z << std::endl;
    cam->setPosition(cam->getPosition() + (newSphere - oldSphere));
    cam->setPitchYaw(pitch, yaw);

    // Handle framebufferresize
    {
        if (inputHandler->frameBufferResized())
        {
            float aspectRatio = inputHandler->getAspectRatio();
            cam->setAspectRatio(aspectRatio);
            inputHandler->frameBufferResizeHandled();
        }
    }

    // Handle keyboard movement
    {
        float velocity = movementSpeed * inputHandler->getKeyDeltaTime();

        VSInputHandler::KEY_FLAGS keyFlags = inputHandler->getKeyFlags();

        if (keyFlags & VSInputHandler::KEY_W)
        {
            glm::vec3 front = cam->getFront();
            front.y = 0.F;
            targetPosition += glm::normalize(front) * velocity;
        }
        if (keyFlags & VSInputHandler::KEY_S)
        {
            glm::vec3 front = cam->getFront();
            front.y = 0.F;
            targetPosition -= glm::normalize(front) * velocity;
        }
        if (keyFlags & VSInputHandler::KEY_A)
        {
            glm::vec3 right = cam->getRight();
            targetPosition -= right * velocity;
        }
        if (keyFlags & VSInputHandler::KEY_D)
        {
            glm::vec3 right = cam->getRight();
            targetPosition += right * velocity;
        }
        if (keyFlags & VSInputHandler::KEY_E)
        {
            glm::vec3 up = cam->getUp();
            targetPosition += up * velocity;
        }
        if (keyFlags & VSInputHandler::KEY_Q)
        {
            glm::vec3 up = cam->getUp();
            targetPosition -= up * velocity;
        }
    }

    // Handle scroll
    {
        float newYOffset = inputHandler->getYScrollOffset();
        // float yOffset = lastYScrollOffset - newYOffset;
        // if (heightAboveMap >= minHeightAboveMap && heightAboveMap <= maxHeightAboveMap && yOffset
        // != 0)
        // {
        //     heightAboveMap -= yOffset;
        //     camHasMoved = true;
        // }
        // if (heightAboveMap <= maxHeightAboveMap)
        // {
        //     heightAboveMap = minHeightAboveMap;
        // }
        // if (heightAboveMap >= maxHeightAboveMap)
        // {
        //     heightAboveMap = maxHeightAboveMap;
        // }
        // if (pitch >= -45.0F && pitch <= 1.0F)
        // {
        //     pitch -= yOffset;
        // }
        // if (pitch <= -45.0F)
        // {
        //     pitch = -45.0F;
        // }
        // if (pitch >= 1.0F)
        // {
        //     pitch = 1.0F;
        // }
        // heightAboveMap = -pitch;
        // cam->setPitchYaw(pitch, 0.F);
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
        // // TODO: Cast ray downwards to find minimal height
        // // TODO: Fix jitter if ray hits different block
        // if (targetPosition != cam->getPosition())
        // {
        //     int width = inputHandler->getDisplayWidth();
        //     int height = inputHandler->getDisplayHeight();
        //     double xPos = (float)width / 2;
        //     double ypos = (float)height / 2;
        //     const auto screenPosCamera = glm::vec3(xPos, double(height) - ypos, 0.F);
        //     const auto screenPosFar = glm::vec3(xPos, double(height) - ypos, 1.F);

        //     const auto tmpView = cam->getViewMatrix();
        //     const auto tmpProj = cam->getProjectionMatrix();
        //     const glm::vec4 viewport = glm::vec4(0.0F, 0.0F, width, height);

        //     const auto worldPosNear = glm::unProject(screenPosCamera, tmpView, tmpProj,
        //     viewport); const auto worldPosFar = glm::unProject(screenPosFar, tmpView, tmpProj,
        //     viewport);

        //     VSChunkManager::VSTraceResult result =
        //         world->getChunkManager()->lineTrace(worldPosNear, worldPosFar);

        //     if (result.bHasHit)
        //     {
        //         // targetPosition.y = result.hitLocation.y + heightAboveMap;
        //         std::cout << (newSphere - oldSphere).x << ", " << (newSphere - oldSphere).y << ",
        //         " << (newSphere - oldSphere).z << std::endl; targetPosition = result.hitLocation
        //         - radius * cam->getFront(); focalPoint = result.hitLocation;
        //     }
        // }
    }

    (void)smoothSpeed;
    // glm::vec3 smoothedPosition = glm::lerp(cam->getPosition(), targetPosition, smoothSpeed);
    // cam->setPosition(smoothedPosition);
}

void VSRTSCameraController::setFocalPoint(glm::vec3 newFocalPoint)
{
    focalPoint = newFocalPoint;
}