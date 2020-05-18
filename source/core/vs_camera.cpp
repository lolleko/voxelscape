#include "core/vs_camera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>

VSCamera::VSCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : front(glm::vec3(0.0F, 0.0F, -1.0F))
    , zoom(ZOOM)
{
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
}

VSCamera::VSCamera(
    float posX,
    float posY,
    float posZ,
    float upX,
    float upY,
    float upZ,
    float yaw,
    float pitch)
    : front(glm::vec3(0.0F, 0.0F, -1.0F))
    , zoom(ZOOM)
{
    this->position = glm::vec3(posX, posY, posZ);
    this->worldUp = glm::vec3(upX, upY, upZ);
    this->yaw = yaw;
    this->pitch = pitch;

    updateCameraVectors();
}

glm::vec3 VSCamera::getPosition() const
{
    return position;
}

glm::mat4 VSCamera::getViewMatrix() const
{
    return cachedViewMatrix;
}

glm::mat4 VSCamera::getProjectionMatrix() const
{
    return cachedProjectionMatrix;
}

glm::mat4 VSCamera::getVPMatrix() const
{
    return cachedVPMatrix;
}

glm::mat4 VSCamera::getMVPMatrixFast(const glm::mat4& model) const
{
    return cachedVPMatrix * model;
}

float VSCamera::getZoom() const 
{
    return zoom;
}

float VSCamera::getPitch() const
{
    return pitch;
}

float VSCamera::getYaw() const
{
    return yaw;
}

glm::vec3 VSCamera::getFront() const
{
    return front;
}

glm::vec3 VSCamera::getRight() const
{
    return right;
}

glm::vec3 VSCamera::getUp() const
{
    return up;
}

void VSCamera::setZoom(float newZoom) 
{
    zoom = newZoom;
    updateCameraVectors();
}

void VSCamera::setPitchYaw(float newPitch, float newYaw)
{
    pitch = newPitch;
    yaw = newYaw;
    updateCameraVectors();
}

void VSCamera::setPosition(glm::vec3 newPosition)
{
    position = newPosition;
    updateCameraVectors();
}

void VSCamera::setAspectRatio(float newAspectRatio)
{
    aspectRatio = newAspectRatio;
    updateCameraVectors();
}

float VSCamera::getZNear() const
{
    return zNear;
}

float VSCamera::getZFar() const
{
    return zFar;
}

void VSCamera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    // Also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(
        front, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more
                           // you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));

    cachedViewMatrix = glm::lookAt(position, position + front, up);
    // aspec ration fixed to 16.9 for now
    cachedProjectionMatrix = glm::perspective(glm::radians(zoom), aspectRatio, zNear, zFar);

    cachedVPMatrix = cachedProjectionMatrix * cachedViewMatrix;
}
