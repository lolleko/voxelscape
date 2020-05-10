#pragma once

#include <cmath>
#include <memory>
#include <array>

// Defines possible basic noise functions, default is PERLIN
// enum NoiseFunction {
//     PERLIN
// };

class VSPerlinNoise;

class VSHeightmap
{
public:
    VSHeightmap(
        unsigned int seed = 42,
        unsigned int maxHeight = 256,
        unsigned int octaves = 1,
        float frequency = 0.01F,
        float amplitude = 1.0F,
        float lacunarity = 2.0F,
        float persistence = 0.5F);

    // Return integer height scaled with maxHeight
    virtual int getVoxelHeight(int x, int y);

    void setMaxHeight(int maxHeight);

    float getHeight(int x, int y);

private:
    VSPerlinNoise* pn;

    unsigned int mMaxHeight;  // maximum height that is allowed, defaults to 256
    unsigned int mOctaves;    // number of octaves used

    float mFrequency;    ///< Frequency ("width") of the first octave of noise (default to 1.0)
    float mAmplitude;    ///< Amplitude ("height") of the first octave of noise (default to 1.0)
    float mLacunarity;   ///< Lacunarity specifies the frequency multiplier between successive
                         ///< octaves (default to 2.0).
    float mPersistence;  ///< Persistence is the loss of amplitude between successive octaves
                         ///< (usually 1/lacunarity)
};