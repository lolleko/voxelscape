#pragma once

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glad/glad.h>

// Default camera values
const float YAW = -90.0F;
const float PITCH = 0.0F;
const float ZOOM = 45.0F;

// An abstract camera class that processes input and calculates the corresponding Euler Angles,
// Vectors and Matrices for use in OpenGL
class VSCamera
{
public:
    // Constructor with vectors
    VSCamera(
        glm::vec3 position = glm::vec3(0.0F, 0.0F, 0.0F),
        glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F),
        float yaw = YAW,
        float pitch = PITCH);

    // Constructor with scalar values
    VSCamera(
        float posX,
        float posY,
        float posZ,
        float upX,
        float upY,
        float upZ,
        float yaw,
        float pitch);

    glm::vec3 getPosition() const;

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix() const;

    glm::mat4 getProjectionMatrix() const;

    glm::mat4 getMVPMatrixFast(const glm::mat4& model) const;

    float getZoom() const;

    float getYaw() const;

    float getPitch() const;

    glm::vec3 getFront() const;

    glm::vec3 getRight() const;
    
    glm::vec3 getUp() const;

    void setZoom(float newZoom);

    void setPitchYaw(float newPitch, float newYaw);

    void setPosition(glm::vec3 newPosition);

private:
    glm::mat4 cachedViewMatrix;
    glm::mat4 cachedProjectionMatrix;
    glm::mat4 cachedVPMatrix;

    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // Euler Angles
    float yaw;
    float pitch;
    // Camera options
    float zoom;

    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};