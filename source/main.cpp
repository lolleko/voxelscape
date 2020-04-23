#include <glad/glad.h>  // Initialize with gladLoadGL()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vs_shader_loader.h"
#include "vs_ui.h"
#include "vs_ui_state.h"
#include "vs_cube.h"

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "Glfw Error " << error << ": " << description << "\n";
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (glfwInit() == 0)
    {
        return 1;
    }

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // 3.0+ only
#endif

    // Create window with graphics context
    const auto width = 1280;
    const auto height = 720;

    GLFWwindow* window =
        glfwCreateWindow(width, height, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
    {
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    bool err = gladLoadGL() == 0;

    if (err)
    {
        std::cerr << "Failed to initialize OpenGL loader!\n";
        return 1;
    }

    VSUI UI;
    // Setup Dear ImGui context
    UI.setup(glsl_version, window);

    const auto* uiState = UI.getState();

    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);

    const auto shaderMap = VSLoadShaders("shaders");
    const auto programID = shaderMap.at("Cube");

    GLuint MVPID = glGetUniformLocation(programID, "MVP");

    GLuint modelID = glGetUniformLocation(programID, "model");

    GLuint lightPosID = glGetUniformLocation(programID, "lightPos");
    GLuint lightColorID = glGetUniformLocation(programID, "lightColor");

    auto testCube = VSCube();

    // Main loop
    while (glfwWindowShouldClose(window) == 0)
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        UI.render();
        auto display_w = 0;
        auto display_h = 0;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        const auto clearColor = uiState->clearColor;
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

        // Clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // recalc camera mvp
        // TODO move ot extra function only recalc if needed
        // shader stuff should be abstracted and controlled by cube?
        glm::mat4 Projection =
            glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

        glm::mat4 View = glm::lookAt(
            uiState->cameraPos,  // Camera is at (4,3,3), in World Space
            glm::vec3(0, 0, 0),  // and looks at the origin
            glm::vec3(0, 1, 0)   // Head is up (set to 0,-1,0 to look upside-down)
        );

        glm::mat4 Model = testCube.getLocalToWorld();
        glm::mat4 MVP = Projection * View * Model;

        glUseProgram(programID);

        glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);

        glUniformMatrix4fv(modelID, 1, GL_FALSE, &Model[0][0]);

        // Light inside camera for now
        glUniform3fv(lightPosID, 1, &uiState->lightPos[0]);

        // Light inside camera for now
        glUniform3fv(lightColorID, 1, &uiState->lightColor[0]);

        testCube.draw();

        UI.draw();
        glfwSwapBuffers(window);
    }

    // Cleanup
    UI.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}