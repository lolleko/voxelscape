#pragma once
#include <glm/fwd.hpp>
#include <vector>
#include "world/vs_world.h"

class VSMinimap
{
public:
    VSMinimap();

    int getWidth();
    int getHeight();
    int getNrComponents();
    unsigned char* getPixelData();
    void updateMinimap(const VSWorld* world);

private:
    // TODO: update to blockID vector
    const std::vector<glm::vec3> blockID2MinimapColor = {/*Air=0*/ {0.F, 0.F, 0.F},
                                                         /*Stone=1*/ {0.3F * 255, 0.3F * 255, 0.3F * 255},
                                                         /*Water=2*/ {0.F, 0.F, 0.5F * 255},
                                                         /*Grass=3*/ {0.F, 0.3F * 255, 0.F},
                                                         /*Wood=4*/ {0.5F * 255, 0.25F * 255, 0.1F * 255},
                                                         /*Sand=5*/ {1.F * 255, 0.9F * 255, 0.5F},
                                                         /*Leaf=6*/ {0.F, 0.6F * 255, 0.F}};

    std::vector<unsigned char> pixels;

    const int width = 128;
    const int height = 128;
    const int nrComponents = 3;
};