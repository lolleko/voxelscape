#include "ui/vs_ui.h"
#include <imgui.h>

#include "core/vs_log.h"
#include "renderer/vs_textureloader.h"
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

    // Prepare Fonts
    debugFont = io.Fonts->AddFontDefault();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
};

void VSUI::cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void VSUI::startRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void VSUI::render()
{
    renderDebugGUI();

    // Check if any window is hovered
    uiState->anyWindowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
}

void VSUI::endRender()
{
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
    ImGui::Checkbox("AO", (bool*)&uiState->bIsAmbientOcclusionEnabled);
    ImGui::Checkbox("Show AO", (bool*)&uiState->bShouldShowAO);
    ImGui::Checkbox("Show UVs", (bool*)&uiState->bShouldShowUV);
    ImGui::Checkbox("Show Normals", (bool*)&uiState->bShouldShowNormals);
    ImGui::Checkbox("Show Light", (bool*)&uiState->bShouldShowLight);
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
