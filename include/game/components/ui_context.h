// Singleton component, Tag::Input, for holding input states.

#pragma once

#include <entt/entity/entity.hpp>
#include <filesystem>
#include <imgui.h>
#include <array>
#include <string>
#include "renderer/vs_textureloader.h"
#include "ui/imgui_impl/imfilebrowser.h"
#include <glm/vec2.hpp>
#include "game/components/unique.h"
#include "game/components/minimap.h"

struct UIContext
{
    bool bShouldUpdateChunks = false;

    bool bShouldGenerateTerrain = false;
    bool bShouldSetGameActive = false;
    bool bIsMenuWorldInitialized = false;
    bool bShouldStartGame = false;
    bool bIsGameWorldRunning = false;

    bool anyWindowHovered = false;

    // Editor Control flow
    bool bShouldSetEditorActive = false;
    bool bShouldResetEditor = false;
    bool bEditorActive = false;
    bool bShouldLoadFromFile = false;
    glm::ivec3 brushSize = {1, 1, 1};
    // This is so the keyboard input does not get processed if there is a file browser active
    bool bFileBrowserActive = false;
    std::filesystem::path loadFilePath = "";
    bool bShouldSaveToFile = false;
    bool bShouldSaveBuilding = false;
    std::filesystem::path saveFilePath = "";
    std::filesystem::path saveBuildingPath = "";
    int editorSelectedBlockID = 0;
    int selectedBiomeType = 0;

    // Menu Control flow
    bool bMenuActive = true;
    bool bGameConfigActive = false;
    bool bShowLoading = false;

    // Game config
    int worldSize = 0;  // 0 = Small, 1 = Medium, 2 = Large

    // Minimap
    Minimap minimap;

    // Very messy names, idk
    // Selected building string
    Unique selectedBuilding = {""};

    // Selected in-world building, not for construction but for deleting, upgrading and showing info
    entt::entity selectedBuildingEntity = entt::null;
    // Show building window if building entitiy is not null
    bool bShowBuildingWindow = false;
    bool bShouldRotateBuilding = false;
    bool bUpgradeBuildingEntity = false;
    bool bDestroyBuildingEntity = false;
    std::string entityDescription = "";

    // Resource counters
    unsigned int woodCount = 200;
    unsigned int stoneCount = 100;
    int populationSpace = 0;

    bool bIsBuildingPreviewInitialized = false;
    bool bIsBuildingPreviewConstructed = false;

    // Textures, move to struct maybe
    unsigned int minimapTexture = 0;
    unsigned int woodResourceTexture = TextureFromFile("resources/textures/tiles/04_log.png");
    unsigned int stoneResourceTexture = TextureFromFile("resources/textures/tiles/01_stone.png");
    unsigned int lumberjackIcon = TextureFromFile("resources/textures/icons/lumberjack_icon.png");
    const std::string lumberjackBuildingName = "building_lumberjack1";
    unsigned int stonemineIcon = TextureFromFile("resources/textures/icons/stonemine_icon.png");
    const std::string stonemineBuildingName = "building_stonemine";
    unsigned int houseIcon = TextureFromFile("resources/textures/icons/house_icon.png");
    const std::string houseBuildingName = "building_house1";
    ImVec4 buttonClickedColor = ImVec4(0.8F, 0.2F, 0.2F, 1.0F);

    ImGui::FileBrowser* loadFileDialog = new ImGui::FileBrowser();
    ImGui::FileBrowser* saveFileDialog =
        new ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename);
    ImGui::FileBrowser* saveBuildingDialog =
        new ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename);

    ImGuiIO& io = ImGui::GetIO();

    ImFont* menuFont = io.Fonts->AddFontFromFileTTF("resources/arial.ttf", 40.F);

    // TODO could be static and omewhere else
    std::string gameWorldName = "Game";
    std::string editorWorldName = "Editor";
    std::string menuWorldName = "Menu";
};
