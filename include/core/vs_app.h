#pragma once

#include <chrono>
#include <map>
#include <string>
#include <thread>

class VSUI;
class VSWorld;
class VSGame;
class VSInputHandler;

struct GLFWwindow;

class VSApp
{
public:
    VSApp();

    int initialize();

    int mainLoop();

    [[nodiscard]] VSWorld* getWorld() const;

    [[nodiscard]] std::string getWorldName() const;

    [[nodiscard]] VSInputHandler* getInputHandler() const;

    void setWorldActive(std::string key);

    void addWorld(std::string key, VSWorld* world);

    [[nodiscard]] VSUI* getUI() const;

    [[nodiscard]] GLFWwindow* getWindow() const;

    [[nodiscard]] std::chrono::high_resolution_clock::time_point getStartTime() const;

    static VSApp* getInstance();

private:
    VSUI* UI;

    std::map<std::string, VSWorld*> worlds;
    std::string worldName;
    VSWorld* world;

    VSGame* game;

    VSInputHandler* inputHandler;

    GLFWwindow* window;

    std::string glslVersion;

    std::chrono::high_resolution_clock::time_point appStart;

    static VSApp* instance;

    int initializeGLFW();
};