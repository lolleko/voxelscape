#pragma once

#include <atomic>
#include <chrono>

#include "core/vs_frame_time_tracker.h"

class VSApp;
class VSWorld;

class VSGame
{
public:
    static const std::string WorldName; 
    void initialize(VSApp* inApp);

    [[nodiscard]] VSWorld* initWorld();

    void gameLoop();
    void handleEditor();

    void quit();

private:
    VSApp* app;

    VSFrameTimeTracker frameTimeTracker;

    std::atomic<bool> bShouldQuit;
};