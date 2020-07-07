#pragma once

#include "core/vs_cameracontroller.h"
#include "world/vs_world.h"

// Forward declarations
class VSCamera;

class VSRTSCameraController : public VSCameraController
{
public:
    VSRTSCameraController(VSCamera* camera, VSWorld* world, VSInputHandler* inputHandler = nullptr);

    void updateCamera(bool handleMouseEvents) override;

    void setCameraRelativeXZ(float relativeX, float relativeZ) override;

    void setFocalPoint(glm::vec3 newFocalPoint);

    void setMaxRadius(float newMaxRadius);

    // Call this when world size has been changed
    void updateWorldBorders();

private:
    bool targetPosChanged = false;

    float targetPitch = -45.F;
    float targetYaw = -90.F;
    float lastYScrollOffset;

    float radius = 50.F;
    float maxRadius = 250.F;
    float minRadius = 5.F;

    float smoothSpeed = 0.125F;
    // float distance = 50.F;
    glm::vec3 focalPoint = {0.F, 0.F, 0.F};
    glm::vec3 targetPosition = {0.F, 0.F, 0.F};

    void handleScroll(bool handleMouseEvents);

    void handleFramebufferResize();

    void handleKeyboard();

    void handleRotation();

    void adaptToFixpoint();
};