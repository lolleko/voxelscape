#pragma once

#include <imgui.h>

struct VSUIState;
struct GLFWwindow;

class VSUI
{
public:
    VSUI();

    void setup(const char* glsl_version, GLFWwindow* window);

    void cleanup();

    void render();
    void renderEditorGUI();
    void renderMainMenu();

    void draw();

    const VSUIState* getState();

    VSUIState* getMutableState();

private:
    VSUIState* uiState;

    ImFont* menuFont;
    ImFont* debugFont;
};