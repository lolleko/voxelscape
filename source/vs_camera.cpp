#include "vs_camera.h"

glm::mat4 VSCamera::updateCameraFromInputs(GLFWwindow* window)
{
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    this->lastTime = currentTime;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Reset mouse position for next frame
    glfwSetCursorPos(window, width/2, height/2);

    // Compute new orientation
    horizontalAngle += mouseSpeed * deltaTime * float(width/2 - xpos);
    verticalAngle   += mouseSpeed * deltaTime * float(height/2 - ypos);

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - 3.14f/2.0f),
        0,
        cos(horizontalAngle - 3.14f/2.0f)
    );

    // Up vector : perpendicular to both direction and right
    glm::vec3 up = glm::cross( right, direction );  

    glm::mat4 view = glm::lookAt(
        position,           // Camera is here
        position+direction, // and looks here : at the same position, plus "direction"
        up                  // Head is up (set to 0,-1,0 to look upside-down)
    );

    return view;
}