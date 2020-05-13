#pragma once

#include <string>

class VSUI;

class VSWorld;

struct GLFWwindow;

class VSApp {
public:
    VSApp();

    int initialize();

    int mainLoop();

    VSWorld* getWorld();
private:
    VSUI* UI;
    VSWorld* world;

    GLFWwindow* window;

    float deltaTime;
    float lastFrame;

    std::string glslVersion;

    int initializeGLFW();
};