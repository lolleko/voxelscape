#pragma once

#include <chrono>
#include <cstdint>
#include <thread>

class VSFrameTimeTracker
{
public:
    void startFrame();

    void endFrame();

    void limitFps(std::uint32_t newFPSLimit);

    void unlimitFps();

    [[nodiscard]] float getDeltaSeconds() const;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds>
        lastFrameStartTime;

    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds>
        frameStartTime;

    float deltaSeconds = 0.F;

    std::uint32_t fpsLimit = 120;

    bool bShouldLimitFPS = false;
};