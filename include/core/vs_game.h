#pragma once

#include <atomic>
#include <chrono>

class VSApp;

class VSGame
{
public:
    void initialize(VSApp* inApp);

    void gameLoop();

    void quit();

private:
    VSApp* app;

    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds>
        lastFrameStartTime;

    /** Delta time in seconds */
    float deltaTime;

    std::atomic<bool> bShouldQuit;
};