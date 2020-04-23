#include <glad/glad.h>  // Initialize with gladLoadGL()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include <iostream>

#include "vs_shader_loader.h"
#include "vs_ui.h"
#include "vs_ui_state.h"

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
    GLFWwindow* window =
        glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
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

    // START http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,
        -1.0f,
        0.0f,
        1.0f,
        -1.0f,
        0.0f,
        0.0f,
        1.0f,
        0.0f,
    };

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint programID = VSLoadShaders(
        "shaders/vertex/SimpleVertexShader.glsl", "shaders/fragment/SimpleFragmentShader.glsl");
    // END Tutorial 2

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
        glClear(GL_COLOR_BUFFER_BIT);

        // START http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
        glUseProgram(programID);

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,  // size
            GL_FLOAT,  // type
            GL_FALSE,  // normalized?
            0,         // stride
            (void*)0   // array buffer offset
        );
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3);  // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);
        // END Tutorial 2

        UI.draw();
        glfwSwapBuffers(window);
    }

    // Cleanup
    UI.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}