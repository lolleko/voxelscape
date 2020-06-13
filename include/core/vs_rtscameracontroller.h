#pragma once

#include "core/vs_cameracontroller.h"
#include "world/vs_world.h"

// Forward declarations
class VSCamera;

class VSRTSCameraController : public VSCameraController
{
public:
    VSRTSCameraController(VSCamera* camera, VSWorld* world, VSInputHandler* inputHandler = nullptr);

    void updateCamera() override;

    void setFocalPoint(glm::vec3 newFocalPoint);

    void handleScroll();
    void handleFramebufferResize();
    void handleKeyboard();
    void handleRotation();
    void adaptToFixpoint();

private:
    bool targetPosChanged = false;

    float targetPitch = -45.F;
    float targetYaw = 0.F;
    float lastYScrollOffset;

    float radius = 50.F;
    float maxRadius = 75.F;
    float minRadius = 1.F;

    float smoothSpeed = 0.125F;
    // float distance = 50.F;
    glm::vec3 focalPoint = {0.F, 0.F, 0.F};
    glm::vec3 targetPosition = {0.F, 0.F, 0.F};
};