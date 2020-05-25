#include "ui/vs_ui.h"
#include <imgui.h>

#include "ui/imgui_impl/imgui_impl_glfw.h"
#include "ui/imgui_impl/imgui_impl_opengl3.h"

#include "ui/vs_ui_state.h"

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
    }
    else if (uiState->bMenuActive)
    {
        renderMainMenu();
    }
    else
    {
        ImGui::ColorEdit3("clear color", (float*)&uiState->clearColor);
        ImGui::Checkbox("wireframe", (bool*)&uiState->isWireframeModeEnabled);
        ImGui::Checkbox("draw chunk border", (bool*)&uiState->bShouldDrawChunkBorder);
        ImGui::Checkbox("freeze frustum", (bool*)&uiState->bShouldFreezeFrustum);
        ImGui::InputInt3("chunk size", (int*)&uiState->chunkSize);
        ImGui::InputInt2("world size", (int*)&uiState->chunkCount);
        if (ImGui::Button("Refresh chunk settings"))
        {
            uiState->bShouldUpdateChunks = true;
        }
        if (ImGui::Button("Generate Heightmap"))
        {
            uiState->bShouldGenerateHeightMap = true;
        }
        if (ImGui::Button("Show Editor"))
        {
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

    // Rendering
    ImGui::Render();
}

void VSUI::renderEditorGUI()
{
    ImGui::ColorEdit3("clear color", (float*)&uiState->clearColor);
    ImGui::Checkbox("wireframe", (bool*)&uiState->isWireframeModeEnabled);
    ImGui::Checkbox("draw chunk border", (bool*)&uiState->bShouldDrawChunkBorder);
    ImGui::InputInt3("chunk size", (int*)&uiState->chunkSize);
    ImGui::InputInt2("world size", (int*)&uiState->chunkCount);
    if (ImGui::Button("Refresh chunk settings"))
    {
        uiState->bShouldUpdateChunks = true;
    }
    if (ImGui::Button("Generate Heightmap"))
    {
        uiState->bShouldGenerateHeightMap = true;
    }
    if (ImGui::Button("Show Game"))
    {
        uiState->bShouldSetGameActive = true;
        uiState->bEditorActive = false;
    }
    if (ImGui::Button("Reset Editor"))
    {
        uiState->bShouldResetEditor = true;
    }
    ImGui::Text("Drawing blocks %d/%d", uiState->activeBlockCount, uiState->totalBlockCount);
    ImGui::Text(
        "Application average %.3f ms/frame (%.1f FPS)",
        1000.0f / ImGui::GetIO().Framerate,
        ImGui::GetIO().Framerate);

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Log");
    ImGui::BeginChild("Scrolling");
    ImGui::Text("%s", uiState->logStream.str().c_str());
    ImGui::EndChild();
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
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Voxelscape");
    if (ImGui::Button("Start Game"))
    {
        // TODO: Start Game
    }
    if (ImGui::Button("Start Editor"))
    {
        uiState->bShouldSetEditorActive = true;
        uiState->bEditorActive = true;
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
