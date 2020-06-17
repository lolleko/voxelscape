#pragma once

#include "core/vs_cameracontroller.h"
#include "world/vs_world.h"

// Forward declarations
class VSCamera;

// Implements first person camera controls
class VSFPCameraController : public VSCameraController
{
public:
    VSFPCameraController(VSCamera* camera, VSWorld* world, VSInputHandler* inputHandler = nullptr);

    void updateCamera(bool handleMouseEvents) override;

private:
    float lastYScrollOffset;
};