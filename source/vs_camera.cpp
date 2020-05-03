#include "vs_camera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

VSCamera::VSCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : front(glm::vec3(0.0F, 0.0F, -1.0F))
    , movementSpeed(SPEED)
    , mouseSensitivity(SENSITIVITY)
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
    , movementSpeed(SPEED)
    , mouseSensitivity(SENSITIVITY)
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

glm::mat4 VSCamera::getMVPMatrixFast(const glm::mat4& model) const
{
    return cachedVPMatrix * model;
}

void VSCamera::processKeyboard(VSCamera_Movement direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)
    {
        position += front * velocity;
    }
    if (direction == BACKWARD)
    {
        position -= front * velocity;
    }
    if (direction == LEFT)
    {
        position -= right * velocity;
    }
    if (direction == RIGHT)
    {
        position += right * velocity;
    }
    if (direction == UP)
    {
        position += up * velocity;
    }
    if (direction == DOWN)
    {
        position -= up * velocity;
    }

    updateCameraVectors();
}

void VSCamera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

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

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void VSCamera::processMouseScroll(float yoffset)
{
    if (zoom >= 1.0F && zoom <= 45.0F)
    {
        zoom -= yoffset;
    }
    if (zoom <= 1.0F)
    {
        zoom = 1.0F;
    }
    if (zoom >= 45.0F)
    {
        zoom = 45.0F;
    }

    updateCameraVectors();
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
    cachedProjectionMatrix = glm::perspective(glm::radians(zoom), 16.F / 9.F, 0.1F, 2000.0F);

    cachedVPMatrix = cachedProjectionMatrix * cachedViewMatrix;
}
