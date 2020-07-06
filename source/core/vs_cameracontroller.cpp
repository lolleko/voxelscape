#include <glm/fwd.hpp>

#include "core/vs_cameracontroller.h"
#include "core/vs_camera.h"
#include "world/vs_chunk_manager.h"

VSCameraController::VSCameraController(
    VSCamera* camera,
    VSWorld* world,
    VSInputHandler* inputHandler)
    : mouseSensitivity(SENSITIVITY)
    , movementSpeed(SPEED)
{
    this->cam = camera;
    this->world = world;
    this->inputHandler = inputHandler;
}

void VSCameraController::setCameraInWorldCoords(glm::vec3 coords)
{
    cameraInWorldCoords = coords;
}

void VSCameraController::setMouseFarInWorldCoords(glm::vec3 mouseFar)
{
    mouseFarInWorldCoords = mouseFar;
}

glm::vec3 VSCameraController::getCameraInWorldCoords() const
{
    return cameraInWorldCoords;
}

glm::vec3 VSCameraController::getMouseFarInWorldCoords() const
{
    return mouseFarInWorldCoords;
}

void VSCameraController::setInputHandler(VSInputHandler* inputHandler)
{
    this->inputHandler = inputHandler;
}

void VSCameraController::setCameraRelativeXZ(float relativeX, float relativeZ)
{
    const auto worldSize = world->getChunkManager()->getWorldSize();
    float oldCameraY = cam->getPosition().y;
    glm::vec3 newPosition = {worldSize.x * relativeX, oldCameraY, worldSize.z * relativeZ};
    cam->setPosition(newPosition);
}