#pragma once

#include <atomic>
#include <chrono>
#include <string>

#include "core/vs_frame_time_tracker.h"

class VSApp;
class VSWorld;

class VSGame
{
public:
    static const std::string WorldName;

    void initialize(VSApp* inApp);

    virtual void initializeGame(VSApp* inApp);

    [[nodiscard]] VSWorld* initWorld();

    void gameLoop();

    virtual void update(float deltaSeconds);

    void handleEditor();

    void quit();

    VSApp* getApp();

private:
    VSApp* app;

    VSFrameTimeTracker frameTimeTracker;

    std::atomic<bool> bShouldQuit;

    void updateInternal(float deltaSeconds);
};