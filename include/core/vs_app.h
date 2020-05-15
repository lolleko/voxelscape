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

    VSWorld* getEditorWorld();
    void setEditorWorldActive();
    void setGameWorldActive();
    VSWorld* getActiveWorld();

    VSUI* getUI();

    GLFWwindow* getWindow();

private:
    VSUI* UI;

    VSWorld* world;
    VSWorld* editorWorld;
    VSWorld* activeWorld;

    VSGame* game;

    GLFWwindow* window;

    std::string glslVersion;

    int initializeGLFW();
};