#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glad/glad.h>

// Defines several possible options for camera movement. Used as abstraction to stay away from
// window-system specific input methods
enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values
const float YAW = -90.0F;
const float PITCH = 0.0F;
const float SPEED = 2.5F;
const float SENSITIVITY = 0.1F;
const float ZOOM = 45.0F;

// An abstract camera class that processes input and calculates the corresponding Euler Angles,
// Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    // TODO: Make private and write getter if necessary, but since we prabably want a world state to
    // access camera members not doing that now
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // Euler Angles
    float yaw;
    float pitch;
    // Camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    // Constructor with vectors
    Camera(
        glm::vec3 position = glm::vec3(0.0F, 0.0F, 0.0F),
        glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F),
        float yaw = YAW,
        float pitch = PITCH);

    // Constructor with scalar values
    Camera(
        float posX,
        float posY,
        float posZ,
        float upX,
        float upY,
        float upZ,
        float yaw,
        float pitch);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix();

    // Processes input received from any keyboard-like input system. Accepts input parameter in the
    // form of camera defined ENUM (to abstract it from windowing systems)
    void processKeyboard(Camera_Movement direction, float deltaTime);

    // Processes input received from a mouse input system. Expects the offset value in both the x
    // and y direction.
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = GL_TRUE);

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical
    // wheel-axis
    void processMouseScroll(float yoffset);

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};