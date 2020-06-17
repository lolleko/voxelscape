#pragma once

#include <glm/fwd.hpp>
#include <memory>
#include "world/vs_world.h"

// Forward declarations
class VSCamera;
class VSInputHandler;
struct GLFWwindow;

const float SPEED = 40.F;
const float SENSITIVITY = 0.1F;

// Implements Observer pattern with VSCameraController being the subject and VSCamera being the
// Obser
class VSCameraController
{
public:
    VSCameraController(VSCamera* camera, VSWorld* world, VSInputHandler* inputHandler = nullptr);
    virtual ~VSCameraController() = default;

    virtual void updateCamera(bool handleMouseEvents) = 0;

    void setEditorBlockID(int blockID);

    void setCameraInWorldCoords(glm::vec3 coords);

    void setMouseFarInWorldCoords(glm::vec3 mouseFar);

    void setInputHandler(VSInputHandler* inputHandler);

    [[nodiscard]] glm::vec3 getCameraInWorldCoords() const;

    [[nodiscard]] glm::vec3 getMouseFarInWorldCoords() const;

protected:
    VSCamera* cam;
    VSWorld* world;
    VSInputHandler* inputHandler;

    bool firstMouse = true;
    float lastX = 0.F;
    float lastY = 0.F;

    float mouseSensitivity;
    float movementSpeed;

    int editorBlockID = 1;
    glm::vec3 cameraInWorldCoords = glm::vec3(0);
    glm::vec3 mouseFarInWorldCoords = glm::vec3(0);
};