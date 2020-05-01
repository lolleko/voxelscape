#include "vs_heightmapgenerator.h"

VSHeightmapGenerator::VSHeightmapGenerator(unsigned int seed) {
    pn = new VSPerlinNoise(seed);
}

#include <iostream>
void VSHeightmapGenerator::generateHeightMap(unsigned int width, unsigned int height) {
    double heightmap[width][height];
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            double x = (double) i / (double) width;
            double y = (double) j / (double) height;

            heightmap[i][j] = pn->noise2d(x, y);
            std::cout << heightmap[i][j] << " ";
        }
        std::cout << "\n";
    }
}