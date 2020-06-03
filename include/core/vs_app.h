#pragma once

#include <map>
#include <memory>
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

    void setWorldActive(std::string key);

    void addWorld(std::string key, VSWorld* world);

    [[nodiscard]] VSUI* getUI() const;

    [[nodiscard]] GLFWwindow* getWindow() const;

    static VSApp* getInstance();

private:
    VSUI* UI;

    std::map<std::string, VSWorld*> worlds;

    VSWorld* world;

    VSGame* game;

    GLFWwindow* window;

    std::string glslVersion;

    static VSApp* instance;

    int initializeGLFW();
};