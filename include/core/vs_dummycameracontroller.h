#pragma once

#include "core/vs_cameracontroller.h"

// Forward declarations
class VSCamera;

// Dummy camera controller with no functionality. Supposed to be used for static menu backgroud
// world.
class VSDummyCameraController : public VSCameraController
{
public:
    VSDummyCameraController(
        VSCamera* camera = nullptr,
        VSWorld* world = nullptr,
        VSInputHandler* inputHandler = nullptr);

    void updateCamera() override;
};