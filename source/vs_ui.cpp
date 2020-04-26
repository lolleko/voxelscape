#include <imgui.h>
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"

#include "vs_ui.h"
#include "vs_ui_state.h"

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
    {
        ImGui::ColorEdit3("clear color", (float*)&uiState->clearColor);
        ImGui::DragFloat3("light pos", (float*)&uiState->lightPos);
        ImGui::ColorEdit3("light color", (float*)&uiState->lightColor);
        ImGui::Checkbox("wireframe", (bool*)&uiState->isWireframeModeEnabled);
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
