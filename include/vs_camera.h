#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

class VSCamera
{
public:
    VSCamera()
    {
        lastTime = glfwGetTime();
    }

    glm::vec3 getPosition()
    {
        return position;
    }

    void setPosition(glm::vec3 pos)
    {
        position = pos;
    }

    float getHorizontalAngle() 
    {
        return horizontalAngle;
    }

    void setHorizontalAngle(float angle)
    {
        horizontalAngle = angle;
    }

    float getVerticalAngle() 
    {
        return verticalAngle;
    }

    void setVerticalAngle(float angle)
    {
        verticalAngle = angle;
    }

    glm::mat4 updateCameraFromInputs(GLFWwindow* window);

private:
    glm::vec3 position = glm::vec3(0.f, 0.f, 3.f);

    float horizontalAngle = 3.14f;

    float verticalAngle = 0.0f;

    float initialFoV = 45.0f;

    float speed = 3.0f;
    float mouseSpeed = 0.05f;
    float lastTime;
};