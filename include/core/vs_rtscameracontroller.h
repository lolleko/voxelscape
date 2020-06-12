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

private:
    // const float maxHeightAboveMap = 50.F;
    // const float minHeightAboveMap = 10.F;
    float heightAboveMap = 50.F;

    float pitch = 45.F;
    float lastYScrollOffset;

    float smoothSpeed = 0.125F;
    glm::vec3 offset = {0.F, 0.F, 0.F};
    glm::vec3 targetPosition = {0.F, 0.F, 0.F};
};