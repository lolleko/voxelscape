#pragma once

#include <atomic>

class VSApp;

class VSGame {
public:
    void initialize(VSApp* inApp);
    void gameLoop();
    void handleEditor();

    void quit();

private:
    VSApp* app;

    float deltaTime;
    float lastFrame;

    std::atomic<bool> bShouldQuit;
};