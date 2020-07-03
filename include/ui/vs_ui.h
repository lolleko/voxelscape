#pragma once

#include <imgui.h>
#include <array>
#include <string>
#include "ui/imgui_impl/imfilebrowser.h"

struct VSUIState;
struct GLFWwindow;

class VSUI
{
public:
    VSUI();

    void setup(const char* glsl_version, GLFWwindow* window);

    void cleanup();

    void startRender();

    void render();
    void renderDebugGUI();

    void endRender();

    void draw();

    const VSUIState* getState();

    VSUIState* getMutableState();

private:
    VSUIState* uiState;

    ImFont* debugFont;
};