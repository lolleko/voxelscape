#pragma once

#include <vector>

// Source: https://solarianprogrammer.com/2012/07/18/perlin-noise-cpp-11/ 
class VSPerlinNoise {
public:
    // Initialize with the reference values for the permutation vector
    VSPerlinNoise();

    // Generate a new permutation vector based on the value of seed
	VSPerlinNoise(unsigned int seed);

	// Get a noise value, for 2D images z can have any value
    // TODO: Maybe reimplement noise2d to be more efficient
    double noise2d(double x, double y);
	double noise3d(double x, double y, double z);
private:
    // Permutation vector
    std::vector<int> p;
    
	double fade(double t);
	double lerp(double t, double a, double b);
	double grad(int hash, double x, double y, double z);
};