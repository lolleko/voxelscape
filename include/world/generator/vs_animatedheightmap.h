#pragma once

#include "world/generator/vs_heightmap.h"

class VSAnimatedHeightmap : public VSHeightmap
{
public:
    VSAnimatedHeightmap(
        unsigned int seed = 42,
        unsigned int maxHeight = 256,
        unsigned int octaves = 1,
        float frequency = 0.01F,
        float amplitude = 1.0F,
        float lacunarity = 2.0F,
        float persistence = 0.5F,
        int framesPerAnimationStep = 1,
        int velocity = 4
    );

    // Return whether or not the heights changed and should be updated
    bool animateStep();

    int getVoxelHeight(int x, int y) override;

private:
    int mOffset;
    int mFrameCount;
    int mFramesPerAnimationStep; // defaults to 1
    int mVelocity; // How much movement between animation steps, defaults to 4

    // TODO: Animate amplutude maybe
    //int amplitudeOffset = 0;
    //const std::array<float, 8> amplitudes = {0.F, 0.2F, 0.4F, 0.6F, 0.8F, -0.5F, -1.F, -0.5F};
};