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

    // Textures, move to struct maybe
    unsigned int minimapTexture;
    unsigned int woodResourceTexture;
    unsigned int stoneResourceTexture;
    unsigned int lumberjackIcon;
    const std::string lumberjackBuildingName = "building_lumberjack";
    unsigned int stonemineIcon;
    const std::string stonemineBuildingName = "building_stonemine";
    ImVec4 buttonClickedColor = ImVec4(0.8F, 0.2F, 0.2F, 1.0F);


    ImFont* menuFont;
    ImFont* debugFont;

    ImGui::FileBrowser* loadFileDialog;
    ImGui::FileBrowser* saveFileDialog;
    ImGui::FileBrowser* saveBuildingDialog;
};