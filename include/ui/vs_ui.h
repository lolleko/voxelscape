#pragma once

#include <imgui.h>
#include <array>
#include <string>
#include "ui/imgui_impl/imfilebrowser.h"
#include "ui/vs_minimap.h"

struct VSUIState;
struct GLFWwindow;

class VSUI
{
public:
    VSUI();

    void setup(const char* glsl_version, GLFWwindow* window);

    void cleanup();

    void render();
    void renderDebugGUI();
    void renderEditorGUI();
    void renderMainMenu();
    void renderGameGUI();
    void renderGameConfigGUI();
    void renderLoading();

    void draw();

    const VSUIState* getState();

    VSUIState* getMutableState();

private:
    VSUIState* uiState;

    unsigned int minimapTexture;
    unsigned int woodResourceTexture;
    unsigned int stoneResourceTexture;

    ImFont* menuFont;
    ImFont* debugFont;

    ImGui::FileBrowser* loadFileDialog;
    ImGui::FileBrowser* saveFileDialog;
    ImGui::FileBrowser* saveBuildingDialog;
};