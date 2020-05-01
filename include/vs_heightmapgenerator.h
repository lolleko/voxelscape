#include "vs_perlinnoise.h"

// Defines possible basic noise functions, default is PERLIN
// enum NoiseFunction {
//     PERLIN
// };

class VSHeightmapGenerator 
{
public:
    VSHeightmapGenerator(unsigned int seed = 42);

    void generateHeightMap(unsigned int width, unsigned int height);

private: 
    VSPerlinNoise *pn;
};