#pragma once

#include <atomic>
#include <chrono>

#include "core/vs_frame_time_tracker.h"

class VSApp;

class VSGame
{
public:
    void initialize(VSApp* inApp);

    void gameLoop();
    void handleEditor();

    void quit();

private:
    VSApp* app;

    VSFrameTimeTracker frameTimeTracker;

    std::atomic<bool> bShouldQuit;
};