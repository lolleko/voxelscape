#include "core/vs_frame_time_tracker.h"
#include <glm/simd/platform.h>

void VSFrameTimeTracker::startFrame()
{
    frameStartTime = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<float> deltaDurationSeconds = frameStartTime - lastFrameStartTime;

    if (firstFrame)
    {
        firstFrame = false;
        deltaSeconds = 0.F;
    }
    else 
    {
        deltaSeconds = deltaDurationSeconds.count();
    }
    lastFrameStartTime = frameStartTime;
}

void VSFrameTimeTracker::endFrame()
{
    if (bShouldLimitFPS)
    {
        const auto frameEndTime = std::chrono::high_resolution_clock::now();
        const auto minimumFrameDuration = std::chrono::nanoseconds(1000000000 / fpsLimit);
        const auto actualFrameDuration = frameEndTime - frameStartTime;
        if (actualFrameDuration < minimumFrameDuration)
        {
            const auto sleepDuration = minimumFrameDuration - actualFrameDuration;
            std::this_thread::sleep_for(sleepDuration);
        }
    }
}

void VSFrameTimeTracker::limitFps(std::uint32_t newFPSLimit)
{
    bShouldLimitFPS = true;
    fpsLimit = newFPSLimit;
}

void VSFrameTimeTracker::unlimitFps()
{
    bShouldLimitFPS = false;
}

float VSFrameTimeTracker::getDeltaSeconds() const
{
    return deltaSeconds;
};