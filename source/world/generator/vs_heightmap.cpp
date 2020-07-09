#include "world/generator/vs_heightmap.h"
#include <glm/gtc/noise.hpp>

VSHeightmap::VSHeightmap(
    unsigned int maxHeight,
    unsigned int octaves,
    float frequency,
    float amplitude,
    float lacunarity,
    float persistence)
{
    mMaxHeight = maxHeight;
    mOctaves = octaves;
    mFrequency = frequency;
    mAmplitude = amplitude;
    mLacunarity = lacunarity;
    mPersistence = persistence;
}

float VSHeightmap::getHeight(int x, int y)
{
    float output = 0.F;
    float denom = 0.F;
    float frequency = mFrequency;
    float amplitude = mAmplitude;

    for (size_t i = 0; i < mOctaves; ++i)
    {
        output += amplitude * glm::perlin(glm::vec2{x * frequency, y * frequency});
        denom += amplitude;

        frequency *= mLacunarity;
        amplitude *= mPersistence;
    }

    return (output / denom);
}

void VSHeightmap::setMaxHeight(int maxHeight)
{
    mMaxHeight = maxHeight;
}

int VSHeightmap::getVoxelHeight(int x, int y)
{
    float height = getHeight(x, y) * (float)mMaxHeight / 2 + (mMaxHeight / 2);
    return static_cast<int>(std::round(height));
}