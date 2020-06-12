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
};