#include "game/voxelscape.h"

#include "core/vs_camera.h"
#include "core/vs_dummycameracontroller.h"
#include "core/vs_rtscameracontroller.h"
#include "game/components/resourceamount.h"
#include "game/components/resources.h"
#include "game/components/unique.h"
#include "game/components/world_context.h"
#include "game/components/player.h"

#include "game/systems/menu_system.h"
#include "game/systems/input_system.h"
#include "game/systems/hover_system.h"
#include "game/systems/placement_system.h"
#include "game/systems/resource_system.h"
#include "game/systems/minimap_system.h"

#include "game/building_loader.h"

void Voxelscape::initializeGame(VSApp* inApp)
{
    (void)inApp;
    BuildingParser::createBuildingFromFile("resources/buildings/lumberjack", buildingRegistry);
    BuildingParser::createBuildingFromFile("resources/buildings/stonemine", buildingRegistry);

    const auto& uiContext = mainRegistry.ctx_or_set<UIContext>();

    // Init player

    // Init Resources
    auto resources = Resources{};

    const auto wood = ResourceAmount{Unique{"lumber"}, 200};
    const auto stone = ResourceAmount{Unique{"stone"}, 100};

    resources.resourceVector.emplace_back(wood);
    resources.resourceVector.emplace_back(stone);

    mainRegistry.set<Player>(resources);

    // Init world context
    mainRegistry.set<WorldContext>(
        getApp()->getWorld(),
        0.F,
        0.F,
        Bounds{
            -getApp()->getWorld()->getChunkManager()->getWorldSize() / 2,
            getApp()->getWorld()->getChunkManager()->getWorldSize() / 2});

    auto* gameWorld = new VSWorld();
    auto* gameCamera = gameWorld->getCamera();
    auto* gameCameraController = new VSRTSCameraController(gameCamera, gameWorld);
    gameWorld->setCameraController(gameCameraController);
    inApp->addWorld(uiContext.gameWorldName, gameWorld);

    VSWorld* editorWorld = new VSWorld();
    editorWorld->getCamera()->setPosition(glm::vec3(-50.F, -5.F, -50.F));
    editorWorld->getCamera()->setPitchYaw(-10.F, 45.F);
    inApp->addWorld(uiContext.editorWorldName, editorWorld);

    // Set menu active on application start
    VSWorld* menuWorld = new VSWorld();
    auto* menuDummyController = new VSDummyCameraController();
    menuWorld->setCameraController(menuDummyController);
    inApp->addWorld(uiContext.menuWorldName, menuWorld);

    inApp->setWorldActive(uiContext.menuWorldName);
}

void Voxelscape::update(float deltaSeconds)
{
    auto& uiContext = mainRegistry.ctx<UIContext>();

    uiContext.anyWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

    updateMenuSystem(mainRegistry);

    // TODO the app/mainloop should be never allowed to change the active world
    // after initailization otherwhise this could crash
    if (getApp()->getWorldName() == uiContext.gameWorldName)
    {
        const auto& prevWorldContext = mainRegistry.ctx<WorldContext>();

        mainRegistry.set<WorldContext>(
            getApp()->getWorld(),
            deltaSeconds,
            prevWorldContext.worldAge + deltaSeconds,
            Bounds{
                -getApp()->getWorld()->getChunkManager()->getWorldSize() / 2,
                getApp()->getWorld()->getChunkManager()->getWorldSize() / 2});

        // Update player resources in UI
        const auto& player = mainRegistry.ctx<Player>();

        for (const auto& resourceAmount : player.resources.resourceVector)
        {
            if (resourceAmount.resource.uuid == "lumber")
            {
                uiContext.woodCount = resourceAmount.amount;
            }
            if (resourceAmount.resource.uuid == "stone")
            {
                uiContext.stoneCount = resourceAmount.amount;
            }
        }

        updateInputSystem(mainRegistry);
        updateHoverSystem(mainRegistry);
        updatePlacementSystem(mainRegistry, buildingRegistry);
        updateResourceSystem(mainRegistry);
        updateMinimapSystem(mainRegistry);
    }
    else if (getApp()->getWorldName() == uiContext.editorWorldName)
    {
        // TODO updateEditorSystem()
    }
}

void Voxelscape::renderUI()
{
    auto& uiContext = mainRegistry.ctx_or_set<UIContext>();

    if (uiContext.bEditorActive)
    {
        renderEditorGUI(uiContext);
    }
    else if (uiContext.bMenuActive)
    {
        renderMainMenu(uiContext);
    }
    else if (uiContext.bGameConfigActive)
    {
        renderGameConfigGUI(uiContext);
    }
    else if (uiContext.bShouldUpdateChunks)
    {
        renderLoading(uiContext);
    }
    else
    {
        renderGameGUI(uiContext);
    }
}

void Voxelscape::renderEditorGUI(UIContext& uiState)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File..."))
        {
            if (ImGui::MenuItem("Load world..."))
            {
                // Open File dialog
                uiState.bFileBrowserActive = true;
                uiState.loadFileDialog->SetTitle("Load scene file");
                uiState.loadFileDialog->SetTypeFilters({".json"});
                uiState.loadFileDialog->Open();
            }
            if (ImGui::MenuItem("Save world..."))
            {
                // Open File dialog
                uiState.bFileBrowserActive = true;
                uiState.saveFileDialog->SetTitle("Save scene file");
                uiState.saveFileDialog->Open();
            }
            if (ImGui::MenuItem("Save Building..."))
            {
                // Open File dialog
                uiState.bFileBrowserActive = true;
                uiState.saveBuildingDialog->SetTitle("Save building");
                uiState.saveBuildingDialog->Open();
            }
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();
    // Select block type to set
    // This needs to be adapted to the new block types obviously
    ImGui::Begin("Editor");
    const char* blockTypes[] = {"Stone", "Water", "Grass", "Wood", "Sand", "Leaf", "Lava"};
    ImGui::Combo(
        "Select block type",
        (int*)&uiState.editorSelectedBlockID,
        blockTypes,
        IM_ARRAYSIZE(blockTypes));

    ImGui::End();

    // Handle file browsers
    uiState.loadFileDialog->Display();
    if (uiState.loadFileDialog->HasSelected())
    {
        // Load scene
        uiState.loadFilePath = uiState.loadFileDialog->GetSelected();
        uiState.bShouldLoadFromFile = true;
        uiState.bFileBrowserActive = false;
        uiState.loadFileDialog->ClearSelected();
    }
    uiState.saveFileDialog->Display();
    if (uiState.saveFileDialog->HasSelected())
    {
        // Save scene
        uiState.saveFilePath = uiState.saveFileDialog->GetSelected();
        uiState.bShouldSaveToFile = true;
        uiState.bFileBrowserActive = false;
        uiState.saveFileDialog->ClearSelected();
    }
    uiState.saveBuildingDialog->Display();
    if (uiState.saveBuildingDialog->HasSelected())
    {
        // Save building
        uiState.saveBuildingPath = uiState.saveBuildingDialog->GetSelected();
        uiState.bShouldSaveBuilding = true;
        uiState.bFileBrowserActive = false;
        uiState.saveBuildingDialog->ClearSelected();
    }
}

void Voxelscape::renderMainMenu(UIContext& uiState)
{
    ImGui::PushFont(uiState.menuFont);
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5F, ImGui::GetIO().DisplaySize.y * 0.5F),
        ImGuiCond_Always,
        ImVec2(0.5F, 0.5F));
    ImGui::SetNextWindowSize(ImVec2(0.F, 0.F));
    ImGui::Begin(
        "Voxelscape",
        0,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    ImGui::Dummy(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.75F, ImGui::GetIO().DisplaySize.y * 0.05F));
    if (ImGui::Button("New Game", ImVec2(ImGui::GetWindowContentRegionWidth(), 0.F)))
    {
        uiState.bGameConfigActive = true;
        uiState.bMenuActive = false;
    }
    if (ImGui::Button("Start Editor", ImVec2(ImGui::GetWindowContentRegionWidth(), 0.F)))
    {
        uiState.bShouldUpdateChunks = true;
        uiState.bShouldResetEditor = true;
        uiState.bShouldSetEditorActive = true;
        uiState.bEditorActive = true;
        uiState.bMenuActive = false;
    }
    ImGui::End();
    ImGui::PopFont();
}

void Voxelscape::renderGameConfigGUI(UIContext& uiState)
{
    ImGui::PushFont(uiState.menuFont);
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5F, ImGui::GetIO().DisplaySize.y * 0.5F),
        ImGuiCond_Always,
        ImVec2(0.5F, 0.5F));
    ImGui::SetNextWindowSize(ImVec2(0.F, 0.F));
    ImGui::Begin(
        "Configure Game",
        0,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    ImGui::Dummy(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.75F, ImGui::GetIO().DisplaySize.y * 0.05F));

    const char* worldSizes[] = {"Small", "Medium", "Large", "Debug"};
    ImGui::Combo("World size", (int*)&uiState.worldSize, worldSizes, IM_ARRAYSIZE(worldSizes));

    // This needs to be adapted to available biome types
    const char* biomeTypes[] = {"Standard", "Mountains", "Desert"};
    ImGui::Combo(
        "Select biome", (int*)&uiState.selectedBiomeType, biomeTypes, IM_ARRAYSIZE(biomeTypes));

    if (ImGui::Button("Start Game", ImVec2(ImGui::GetWindowContentRegionWidth(), 0.F)))
    {
        uiState.bShouldSetGameActive = true;
        uiState.bShouldUpdateChunks = true;
        uiState.bShouldGenerateTerrain = true;
        uiState.bGameConfigActive = false;
    }
    ImGui::End();
    ImGui::PopFont();
}

void Voxelscape::renderGameGUI(UIContext& uiState)
{
    float menuBarHeight = 0.F;
    if (ImGui::BeginMainMenuBar())
    {
        ImGui::MenuItem("Dummy");
        ImGui::Separator();
        ImGui::Image((void*)(intptr_t)uiState.woodResourceTexture, ImVec2(20, 20));
        ImGui::Text("%i", uiState.woodCount);

        ImGui::Image((void*)(intptr_t)uiState.stoneResourceTexture, ImVec2(20, 20));
        ImGui::Text("%i", uiState.stoneCount);
        menuBarHeight = ImGui::GetFontSize() + 2 * ImGui::GetStyle().FramePadding.y;
    }
    ImGui::EndMainMenuBar();

    // Building selection
    ImGui::SetNextWindowPos(ImVec2(0, menuBarHeight), ImGuiCond_Always, ImVec2(0.F, 0.F));
    ImGui::SetNextWindowSize(ImVec2(0.F, 0.F));
    ImGui::Begin("Select building", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    if (ImGui::IsWindowCollapsed())
    {
        // Do not set building if collapsed
        uiState.selectedBuilding = {""};
    }
    // Building types
    bool styleColorPushed = false;
    if (uiState.selectedBuilding.uuid == uiState.lumberjackBuildingName)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, uiState.buttonClickedColor);
        styleColorPushed = true;
    }
    if (ImGui::ImageButton((void*)(intptr_t)uiState.lumberjackIcon, ImVec2(64, 64)))
    {
        // Set lumberjack building name
        uiState.selectedBuilding.uuid = uiState.lumberjackBuildingName;
    }
    if (styleColorPushed)
    {
        ImGui::PopStyleColor();
        styleColorPushed = false;
    }
    ImGui::SameLine();
    if (uiState.selectedBuilding.uuid == uiState.stonemineBuildingName)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, uiState.buttonClickedColor);
        styleColorPushed = true;
    }
    if (ImGui::ImageButton((void*)(intptr_t)uiState.stonemineIcon, ImVec2(64, 64)))
    {
        // Set stonemine building name
        uiState.selectedBuilding.uuid = uiState.stonemineBuildingName;
    }
    if (styleColorPushed)
    {
        ImGui::PopStyleColor();
        styleColorPushed = false;
    }
    ImGui::End();

    // Minimap
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x, menuBarHeight), ImGuiCond_Always, ImVec2(1.F, 0.F));
    ImGui::SetNextWindowSize(ImVec2(0.F, 0.F));
    ImGui::Begin("Minimap", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    if (uiState.minimap.bIsUpdateCompleted)
    {
        uiState.minimapTexture = TextureFromData(
            uiState.minimap.pixels.data(),
            uiState.minimap.width,
            uiState.minimap.height,
            uiState.minimap.nrComponents);
        uiState.minimap.bIsUpdateCompleted = false;
    }
    if (ImGui::ImageButton(
            (void*)(intptr_t)uiState.minimapTexture,
            ImVec2(256, 256),
            ImVec2(0, 0),
            ImVec2(1, 1),
            /*frame_padding*/ 1))
    {
        float relativeX = (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) /
                          (ImGui::GetItemRectMax().x - ImGui::GetItemRectMin().x);
        float relativeY = (ImGui::GetMousePos().y - ImGui::GetItemRectMin().y) /
                          (ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y);
        uiState.minimap.relativeClickPosition = {relativeX, relativeY};
        uiState.minimap.bWasClicked = true;
    }
    ImGui::End();
}

void Voxelscape::renderLoading(UIContext& uiState)
{
    ImGui::PushFont(uiState.menuFont);
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5F, ImGui::GetIO().DisplaySize.y * 0.5F),
        ImGuiCond_Always,
        ImVec2(0.5F, 0.5F));
    ImGui::SetNextWindowSize(ImVec2(0.F, 0.F));
    ImGui::Begin(
        "Loading",
        0,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar);

    ImGui::Text("Loading %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);

    ImGui::End();
    ImGui::PopFont();
}