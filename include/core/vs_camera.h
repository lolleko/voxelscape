#pragma once

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glad/glad.h>

// Default camera values
constexpr float YAW = -90.0F;
constexpr float PITCH = 0.0F;
constexpr float ZOOM = 45.0F;

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

    [[nodiscard]] glm::vec3 getPosition() const;

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    [[nodiscard]] glm::mat4 getViewMatrix() const;

    [[nodiscard]] glm::mat4 getProjectionMatrix() const;

    [[nodiscard]] glm::mat4 getVPMatrix() const;

    [[nodiscard]] glm::mat4 getMVPMatrixFast(const glm::mat4& model) const;

    [[nodiscard]] float getZoom() const;

    [[nodiscard]] float getYaw() const;

    [[nodiscard]] float getPitch() const;

    [[nodiscard]] glm::vec3 getFront() const;

    [[nodiscard]] glm::vec3 getRight() const;

    [[nodiscard]] glm::vec3 getUp() const;

    void setZoom(float newZoom);

    void setPitchYaw(float newPitch, float newYaw);

    void setPosition(glm::vec3 newPosition);

    void setAspectRatio(float newAspectRatio);

    [[nodiscard]] float getZNear() const;

    [[nodiscard]] float getZFar() const;

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
    // Current aspect ratio
    float aspectRatio = 16.F / 9.F;

    float zNear = 0.1F;
    float zFar = 1000.F;

    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};