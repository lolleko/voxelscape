#pragma once

#include <map>
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

    [[nodiscard]] std::string getWorldName() const;

    void setWorldActive(std::string key);

    void addWorld(std::string key, VSWorld* world);

    [[nodiscard]] VSUI* getUI() const;

    [[nodiscard]] GLFWwindow* getWindow() const;

    static VSApp* getInstance();

private:
    VSUI* UI;

    std::map<std::string, VSWorld*> worlds;
    std::string worldName;
    VSWorld* world;

    VSGame* game;

    GLFWwindow* window;

    std::string glslVersion;

    static VSApp* instance;

    int initializeGLFW();
};