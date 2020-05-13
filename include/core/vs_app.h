#pragma once

#include <string>
#include <thread>

class VSUI;
class VSWorld;
class VSGame;

struct GLFWwindow;

class VSApp {
public:
    VSApp();

    int initialize();

    int mainLoop();

    VSWorld* getWorld();

    VSUI* getUI();

    GLFWwindow* getWindow();

private:
    VSUI* UI;

    VSWorld* world;

    VSGame* game;

    GLFWwindow* window;

    std::string glslVersion;

    int initializeGLFW();
};