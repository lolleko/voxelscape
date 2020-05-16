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

    static VSApp* getInstance();

private:
    VSUI* UI;

    VSWorld* world;

    VSGame* game;

    GLFWwindow* window;

    std::string glslVersion;

    static VSApp* instance;

    int initializeGLFW();
};