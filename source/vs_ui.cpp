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
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin(
            "Hello, world!");  // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");  // Display some text (you can use a format
                                                   // strings too)
        ImGui::Checkbox("Another Window", &uiState->showOtherWindow);

        ImGui::SliderFloat(
            "float", &f, 0.0f, 1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3(
            "clear color", (float*)&uiState->clearColor);  // Edit 3 floats representing a color

        if (ImGui::Button("Button"))  // Buttons return true when clicked (most widgets return
                                      // true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text(
            "Application average %.3f ms/frame (%.1f FPS)",
            1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (uiState->showOtherWindow)
    {
        ImGui::Begin(
            "Another Window",
            &uiState->showOtherWindow);  // Pass a pointer to our bool variable (the window will
                                         // have a closing button that will clear the bool when
                                         // clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
        {
            uiState->showOtherWindow = false;
        }
        ImGui::End();
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
