#include "ui/vs_ui.h"
#include <imgui.h>

#include "core/vs_log.h"
#include "ui/imgui_impl/imgui_impl_glfw.h"
#include "ui/imgui_impl/imgui_impl_opengl3.h"

#include "ui/vs_ui_state.h"
#include "world/generator/vs_terrain.h"

VSUI::VSUI()
    : uiState(new VSUIState){};

void VSUI::setup(const char* glsl_version, GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Adapt style
    ImGuiStyle* style = &ImGui::GetStyle();
    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 5.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 4.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 3.0f;
    style->WindowTitleAlign = ImVec2(0.5F, 0.5F);

    // Init file browsers
    loadFileDialog = new ImGui::FileBrowser();
    loadFileDialog->SetTitle("Load scene file");
    // TODO: Define file ending
    loadFileDialog->SetTypeFilters({".json"});
    saveFileDialog = new ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename);
    saveFileDialog->SetTitle("Save scene file");
    saveBuildingDialog = new ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename);
    saveBuildingDialog->SetTitle("Save building");

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Prepare Fonts
    debugFont = io.Fonts->AddFontDefault();
    menuFont = io.Fonts->AddFontFromFileTTF("resources/arial.ttf", 40.F);
};

void VSUI::cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void VSUI::render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a
    // named window.
    if (uiState->bEditorActive)
    {
        renderEditorGUI();
        renderDebugGUI();
    }
    else if (uiState->bMenuActive)
    {
        renderMainMenu();
    }
    else
    {
        renderDebugGUI();
    }

    // Rendering
    ImGui::Render();
}

void VSUI::renderDebugGUI()
{
    ImGui::ColorEdit3("clear color", (float*)&uiState->clearColor);
    ImGui::DragFloat3("sun dir", (float*)&uiState->directLightDir, 0.01F, -1.F, 1.F);
    ImGui::Checkbox("wireframe", (bool*)&uiState->isWireframeModeEnabled);
    ImGui::Checkbox("draw chunk border", (bool*)&uiState->bShouldDrawChunkBorder);
    ImGui::Checkbox("freeze frustum", (bool*)&uiState->bShouldFreezeFrustum);
    ImGui::Checkbox("shadows", (bool*)&uiState->bAreShadowsEnabled);
    // TODO ImGui::Checkbox("AO", (bool*)&uiState->bIsAmbientOcclusionEnabled);
    ImGui::InputInt3("chunk size", (int*)&uiState->chunkSize);
    ImGui::InputInt2("world size", (int*)&uiState->chunkCount);
    if (ImGui::Button("Refresh chunk settings"))
    {
        uiState->bShouldUpdateChunks = true;
    }
    // This needs to be adapted to available biome types
    const char* biomeTypes[] = {"Mountains", "Desert"};
    ImGui::Combo("Select biome", (int*)&uiState->bBiomeType, biomeTypes, IM_ARRAYSIZE(biomeTypes));
    if (ImGui::Button("Generate Terrain"))
    {
        uiState->bShouldGenerateTerrain = true;
    }
    if (ImGui::Button("Show Editor"))
    {
        uiState->bShouldUpdateChunks = true;
        uiState->bShouldResetEditor = true;
        uiState->bShouldSetEditorActive = true;
        uiState->bEditorActive = true;
    }
    ImGui::Text(
        "Blocks Total; Visible; Drawn: %d; %d; %d",
        uiState->totalBlockCount,
        uiState->visibleBlockCount,
        uiState->drawnBlockCount);
    ImGui::Text("Drawcalls %d/64", uiState->drawCallCount);
    ImGui::Text(
        "Application average %.3f ms/frame (%.1f FPS)",
        1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate);

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Log");
    ImGui::BeginChild("Scrolling");
    ImGui::Text("%s", uiState->logStream.str().c_str());
    ImGui::EndChild();
}

void VSUI::renderEditorGUI()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File..."))
        {
            if (ImGui::MenuItem("Load world..."))
            {
                // Open File dialog
                uiState->bFileBrowserActive = true;
                loadFileDialog->Open();
            }
            if (ImGui::MenuItem("Save world..."))
            {
                // Open File dialog
                uiState->bFileBrowserActive = true;
                saveFileDialog->Open();
            }
            if (ImGui::MenuItem("Save Building..."))
            {
                // Open File dialog
                uiState->bFileBrowserActive = true;
                saveBuildingDialog->Open();
            }
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();
    // Select block type to set
    // This needs to be adapted to the new block types obviously
    ImGui::Begin("Editor");
    const char* blockTypes[] = {"Stone", "Water", "Grass", "Wood", "Sand", "Leaf"};
    if (ImGui::Combo(
            "Select block type", (int*)&uiState->bSetBlockID, blockTypes, IM_ARRAYSIZE(blockTypes)))
    {
        uiState->bShouldUpdateBlockID = true;
    }
    ImGui::End();

    // Handle file browsers
    loadFileDialog->Display();
    if (loadFileDialog->HasSelected())
    {
        // Load scene
        uiState->loadFilePath = loadFileDialog->GetSelected();
        uiState->bShouldLoadFromFile = true;
        uiState->bFileBrowserActive = false;
        loadFileDialog->ClearSelected();
    }
    saveFileDialog->Display();
    if (saveFileDialog->HasSelected())
    {
        // Save scene
        uiState->saveFilePath = saveFileDialog->GetSelected();
        uiState->bShouldSaveToFile = true;
        uiState->bFileBrowserActive = false;
        saveFileDialog->ClearSelected();
    }
    saveBuildingDialog->Display();
    if (saveBuildingDialog->HasSelected())
    {
        // Save building
        uiState->saveBuildingPath = saveBuildingDialog->GetSelected();
        uiState->bShouldSaveBuilding = true;
        uiState->bFileBrowserActive = false;
        saveBuildingDialog->ClearSelected();
    }
}

void VSUI::renderMainMenu()
{
    ImGui::PushFont(menuFont);
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
    if (ImGui::Button("Start Game", ImVec2(ImGui::GetWindowContentRegionWidth(), 0.F)))
    {
        // TODO: Start Game
        uiState->bShouldSetGameActive = true;
        uiState->bEditorActive = false;
        uiState->bMenuActive = false;
    }
    if (ImGui::Button("Start Editor", ImVec2(ImGui::GetWindowContentRegionWidth(), 0.F)))
    {
        uiState->bShouldUpdateChunks = true;
        uiState->bShouldResetEditor = true;
        uiState->bShouldSetEditorActive = true;
        uiState->bEditorActive = true;
        uiState->bMenuActive = false;
    }
    ImGui::End();
    ImGui::PopFont();
}

void VSUI::draw()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

const VSUIState* VSUI::getState()
{
    return uiState;
}

VSUIState* VSUI::getMutableState()
{
    return uiState;
}
