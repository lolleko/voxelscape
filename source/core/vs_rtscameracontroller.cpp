#define _USE_MATH_DEFINES
#include "core/vs_rtscameracontroller.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <glm/ext/matrix_projection.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/compatibility.hpp>
#include "core/vs_camera.h"
#include "core/vs_debug_draw.h"
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
    targetPosition = cam->getPosition();
    cam->setPitchYaw(targetPitch, targetYaw);
    movementSpeed = 100.F;
    lastYScrollOffset = radius;
}

void VSRTSCameraController::updateCamera(bool handleMouseEvents)
{
    if (!inputHandler)
    {
        // Cannot update without inputs
        return;
    }

    targetPosChanged = false;

    // Handeling sequence must not be changed

    handleScroll(handleMouseEvents);

    // Calculate old sphere coordinates offset
    glm::vec3 oldSphere;
    {
        float pitchRad = targetPitch * (M_PI / 180.F);
        float yawRad = -(targetYaw - 90.F) * (M_PI / 180.F);
        // Y is height axis
        oldSphere.z = radius * std::sin(pitchRad) * std::cos(yawRad);
        oldSphere.x = radius * std::sin(pitchRad) * std::sin(yawRad);
        oldSphere.y = radius * std::cos(pitchRad);
    }

    handleRotation();

    handleFramebufferResize();

    handleKeyboard();

    float smoothedYaw = glm::lerp(cam->getYaw(), targetYaw, smoothSpeed);

    // Calculate new sphere coordinates
    glm::vec3 newSphere;
    {
        float pitchRad = targetPitch * (M_PI / 180.F);
        float yawRad = -(targetYaw - 90.F) * (M_PI / 180.F);
        // Y is height axis
        newSphere.z = radius * std::sin(pitchRad) * std::cos(yawRad);
        newSphere.x = radius * std::sin(pitchRad) * std::sin(yawRad);
        newSphere.y = radius * std::cos(pitchRad);
    }

    if (newSphere != oldSphere)
    {
        targetPosition += (newSphere - oldSphere);
        targetPosChanged = true;
    }

    // Calculate mouse coords in world space
    {
        double xPos = inputHandler->getMouseX();
        double yPos = inputHandler->getMouseY();
        int width = inputHandler->getDisplayWidth();
        int height = inputHandler->getDisplayHeight();
        const auto screenPosCamera = glm::vec3(xPos, double(height) - yPos, 0.F);
        const auto screenPosFar = glm::vec3(xPos, double(height) - yPos, 1.F);

        const auto tmpView = cam->getViewMatrix();
        const auto tmpProj = cam->getProjectionMatrix();
        const glm::vec4 viewport = glm::vec4(0.0F, 0.0F, width, height);

        const auto worldPosNear = glm::unProject(screenPosCamera, tmpView, tmpProj, viewport);
        const auto worldPosFar = glm::unProject(screenPosFar, tmpView, tmpProj, viewport);

        setCameraInWorldCoords(worldPosNear);
        setMouseFarInWorldCoords(worldPosFar);
    }

    adaptToFixpoint();

    glm::vec3 smoothedPosition = glm::lerp(cam->getPosition(), targetPosition, smoothSpeed);
    cam->setPitchYaw(targetPitch, smoothedYaw);
    cam->setPosition(smoothedPosition);
}

void VSRTSCameraController::setFocalPoint(glm::vec3 newFocalPoint)
{
    focalPoint = newFocalPoint;

    if (!inputHandler)
    {
        return;
    }

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
        targetPosition = result.hitLocation + cam->getFront() * radius;
        cam->setPosition(targetPosition);
    }
}

void VSRTSCameraController::setMaxRadius(float newMaxRadius)
{
    maxRadius = newMaxRadius;
};

void VSRTSCameraController::handleScroll(bool handleMouseEvents)
{
    float newYOffset = inputHandler->getYScrollOffset();
    if (handleMouseEvents)
    {
        float yOffset = lastYScrollOffset - newYOffset;
        float oldRadius = radius;
        if (radius >= minRadius && radius <= maxRadius && yOffset != 0)
        {
            radius -= yOffset;
        }
        if (radius <= minRadius)
        {
            radius = minRadius;
        }
        if (radius >= maxRadius)
        {
            radius = maxRadius;
        }
        if (oldRadius != radius)
        {
            targetPosition -= cam->getFront() * (radius - oldRadius);
            targetPosChanged = true;
        }
    }
    lastYScrollOffset = newYOffset;
}

void VSRTSCameraController::handleFramebufferResize()
{
    if (inputHandler->frameBufferResized())
    {
        float aspectRatio = inputHandler->getAspectRatio();
        cam->setAspectRatio(aspectRatio);
        inputHandler->frameBufferResizeHandled();
    }
}

void VSRTSCameraController::handleKeyboard()
{
    float velocity = movementSpeed * inputHandler->getKeyDeltaTime();

    VSInputHandler::KEY_FLAGS keyFlags = inputHandler->getKeyFlags();

    if (keyFlags & VSInputHandler::KEY_W)
    {
        glm::vec3 front = cam->getFront();
        front.y = 0.F;
        targetPosition += glm::normalize(front) * velocity;
        targetPosChanged = true;
    }
    if (keyFlags & VSInputHandler::KEY_S)
    {
        glm::vec3 front = cam->getFront();
        front.y = 0.F;
        targetPosition -= glm::normalize(front) * velocity;
        targetPosChanged = true;
    }
    if (keyFlags & VSInputHandler::KEY_A)
    {
        glm::vec3 right = cam->getRight();
        targetPosition -= right * velocity;
        targetPosChanged = true;
    }
    if (keyFlags & VSInputHandler::KEY_D)
    {
        glm::vec3 right = cam->getRight();
        targetPosition += right * velocity;
        targetPosChanged = true;
    }

    if (keyFlags & VSInputHandler::KEY_Q)
    {
        targetYaw += velocity;
        targetPosChanged = true;
    }
    if (keyFlags & VSInputHandler::KEY_E)
    {
        targetYaw -= velocity;
        targetPosChanged = true;
    }
}

void VSRTSCameraController::handleRotation()
{
    float xPos = inputHandler->getMouseX();
    float yPos = inputHandler->getMouseY();
    if (inputHandler->isMiddleMouseClicked())
    {
        if (firstMouse)
        {
            lastX = xPos;
            lastY = yPos;
            firstMouse = false;
        }
        else
        {
            float xOffset = xPos - lastX;
            float yOffset = lastY - yPos;  // reversed since y-coordinates go from bottom to top

            lastX = xPos;
            lastY = yPos;

            xOffset *= mouseSensitivity;
            yOffset *= mouseSensitivity;

            targetYaw += xOffset;
        }
    }
    else
    {
        lastX = xPos;
        lastY = yPos;
    }
}

void VSRTSCameraController::adaptToFixpoint()
{
    // TODO: Cast ray downwards to find minimal height
    if (targetPosChanged)
    {
        // Restrict position to map
        glm::vec3 worldSize = world->getChunkManager()->getWorldSize();
        if (targetPosition.x <= -worldSize.x / 2)
        {
            targetPosition.x = -worldSize.x / 2;
        }
        if (targetPosition.x >= worldSize.x / 2)
        {
            targetPosition.x = worldSize.x / 2;
        }
        if (targetPosition.z <= -worldSize.z / 2)
        {
            targetPosition.z = -worldSize.z / 2;
        }
        if (targetPosition.z >= worldSize.z / 2)
        {
            targetPosition.z = worldSize.z / 2;
        }

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
            world->getDebugDraw()->drawSphere(result.hitLocation, 0.5F, {255, 0, 0});
            targetPosition.y =
                result.hitLocation.y + radius * std::sin(-targetPitch * (M_PI / 180.F));
        }
    }
}

void VSRTSCameraController::setCameraRelativeXZ(float relativeX, float relativeZ)
{
    const auto worldSize = world->getChunkManager()->getWorldSize();

    float absoluteX = (relativeX - 0.5) * worldSize.x;
    float absoluteZ = (relativeZ - 0.5) * worldSize.z;
    int blockX = std::round(absoluteX);
    int blockZ = std::round(absoluteZ);
    for (int y = -worldSize.y / 2; y < worldSize.y / 2; y++)
    {
        glm::ivec3 blockCoordinates = {blockX, y, blockZ};
        if (world->getChunkManager()->getBlock(blockCoordinates) > 0)
        {
            const auto newPosition =
                glm::vec3({absoluteX, y, absoluteZ}) - cam->getFront() * radius;
            targetPosition = newPosition;
            adaptToFixpoint();
            cam->setPosition(targetPosition);
        }
    }
}
