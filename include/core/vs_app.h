#pragma once

#include <string>
#include <thread>

class VSUI;
class VSWorld;
class VSGame;

struct GLFWwindow;

class VSApp
{
public:
    VSApp();

    int initialize();

    int mainLoop();

    [[nodiscard]] VSWorld* getWorld() const;

    VSWorld* getEditorWorld();
    void setEditorWorldActive();
    void setGameWorldActive();
    VSWorld* getActiveWorld();

    [[nodiscard]] VSUI* getUI() const;

    [[nodiscard]] GLFWwindow* getWindow() const;

    static VSApp* getInstance();

private:
    VSUI* UI;

    VSWorld* world;
    VSWorld* editorWorld;
    VSWorld* menuWorld;
    VSWorld* activeWorld;

    VSGame* game;

    GLFWwindow* window;

    std::string glslVersion;

    static VSApp* instance;

    int initializeGLFW();
};