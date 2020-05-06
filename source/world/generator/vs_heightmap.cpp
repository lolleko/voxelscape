#include "world/generator/vs_heightmap.h"
#include "world/generator/vs_perlinnoise.h"

VSHeightmap::VSHeightmap(unsigned int seed, unsigned int maxHeight, unsigned int octaves, float frequency, float amplitude, float lacunarity, float persistence) {
    pn = new VSPerlinNoise(seed);
    mMaxHeight = maxHeight;
    mOctaves = octaves;
    mFrequency = frequency;
    mAmplitude = amplitude;
    mLacunarity = lacunarity;
    mPersistence = persistence;
}

float VSHeightmap::getHeight(int x, int y) {
    float output = 0.F;
    float denom  = 0.F;
    float frequency = mFrequency;
    float amplitude = mAmplitude;

    for (size_t i = 0; i < mOctaves; ++i) {
        output += (amplitude * pn->noise2d(x * frequency, y * frequency));
        denom += amplitude;

        frequency *= mLacunarity;
        amplitude *= mPersistence;
    }

    return (output / denom);
}

int VSHeightmap::getVoxelHeight(int x, int y) {
    float height = getHeight(x, y) * (float)mMaxHeight;
    return static_cast<int>(std::round(height));
}