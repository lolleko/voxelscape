#include "world/generator/vs_animatedheightmap.h"
#include "world/generator/vs_heightmap.h"

VSAnimatedHeightmap::VSAnimatedHeightmap(unsigned int seed,
        unsigned int maxHeight,
        unsigned int octaves,
        float frequency,
        float amplitude,
        float lacunarity,
        float persistence,
        int framesPerAnimationStep,
        int velocity)
    : VSHeightmap(seed, maxHeight, octaves, frequency, amplitude, lacunarity, persistence)
{
    mOffset = 0;
    mFrameCount = 0;
    mFramesPerAnimationStep = framesPerAnimationStep;
    mVelocity = velocity;
}

int VSAnimatedHeightmap::getVoxelHeight(int x, int y)
{
    x += mOffset;
    y += mOffset;
    int height = VSHeightmap::getVoxelHeight(x, y);
    return height;
}

bool VSAnimatedHeightmap::animateStep()
{
    mFrameCount += 1;
    if (mFrameCount == mFramesPerAnimationStep)
    {
        mFrameCount = 0;
        mOffset += mVelocity;
        return true;
    }
    return false;
}