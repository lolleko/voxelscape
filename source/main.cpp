#include <glad/glad.h>  // Initialize with gladLoadGL()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include <spdlog/common.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "renderer/vs_drawable.h"
#include "renderer/vs_shader.h"
#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"
#include "world/vs_cube.h"
#include "core/vs_camera.h"
#include "core/vs_cameracontroller.h"
#include "core/vs_log.h"
#include "renderer/vs_model.h"
#include "world/vs_skybox.h"
#include "renderer/vs_textureloader.h"
#include "world/generator/vs_heightmap.h"
#include "world/generator/vs_animatedheightmap.h"
#include "world/vs_chunk.h"
#include "world/vs_world.h"

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "Glfw Error " << error << ": " << description << "\n";
}

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// timing
float deltaTime = 0.0F;  // time between current frame and last frame
float lastFrame = 0.0F;

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

    VSUI UI;
    // initialize logger
    VSLog::init(UI.getMutableState()->logStream);
    VSLog::Log(VSLog::Category::Core, VSLog::Level::info, "Succesfully initialized logger");

    GLFWwindow* window = glfwCreateWindow(width, height, "Voxelscape", nullptr, nullptr);
    if (window == nullptr)
    {
        VSLog::Log(VSLog::Category::Core, VSLog::Level::critical, "Failed to create GLFW window");
        return 1;
    }

    // TODO move world init down once this input mess has been solved
    auto* world = new VSWorld();

    glfwSetWindowUserPointer(window, world);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        auto* world = static_cast<VSWorld*>(glfwGetWindowUserPointer(window));
        world->getCameraController()->processFramebufferResize(window, width, height);
    });
    // TODO these callbacks need to be part of a sperate controller class
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        // TODO messy
        auto* world = static_cast<VSWorld*>(glfwGetWindowUserPointer(window));
        world->getCameraController()->processMouseMovement(window, xpos, ypos);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        // TODO messy
        auto* world = static_cast<VSWorld*>(glfwGetWindowUserPointer(window));
        world->getCameraController()->processMouseButton(window, button, action, mods);
    });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        // TODO messy
        auto* world = static_cast<VSWorld*>(glfwGetWindowUserPointer(window));
        world->getCameraController()->processMouseScroll(window, xoffset, yoffset);
    });
    glfwSwapInterval(1);  // Enable vsync

    VSLog::Log(
        VSLog::Category::Core,
        VSLog::Level::info,
        "Succesfully setup GLFW window and opengl context");

    bool err = gladLoadGL() == 0;
    if (err)
    {
        VSLog::Log(
            VSLog::Category::Core, VSLog::Level::critical, "Failed to initialize OpenGL loader");
        return 1;
    }
    VSLog::Log(VSLog::Category::Core, VSLog::Level::info, "Succesfully initialized OpenGL loader");

    // Setup Dear ImGui context
    UI.setup(glsl_version, window);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    // auto monkeyModel = std::make_shared<VSModel>("monkey.obj");
    // auto monkeyShader = std::make_shared<VSShader>("Monkey");

    auto skybox = new VSSkybox();
    auto skyboxShader = std::make_shared<VSShader>("Skybox");
    const auto worldSize = world->getWorldSize();
    // Messy, segfaults if world gets resized
    VSAnimatedHeightmap animatedHm =
        VSAnimatedHeightmap(42, worldSize.y, 2, 0.02F, 4.F, 2.0F, 0.5F);

    world->initializeChunks();

    world->addDrawable(skybox, skyboxShader);

    VSLog::Log(VSLog::Category::Core, VSLog::Level::info, "Starting main loop");

    // Main loop
    while (glfwWindowShouldClose(window) == 0)
    {
        // per-frame time logic
        // --------------------
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        world->getCameraController()->processKeyboardInput(window, deltaTime);

        glfwPollEvents();

        if (UI.getState()->isWireframeModeEnabled)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // Start the Dear ImGui frame
        UI.render();

        auto display_w = 0;
        auto display_h = 0;

        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        const auto clearColor = UI.getState()->clearColor;
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

        // Clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update world state with ui state
        if (UI.getState()->bShouldUpdateChunks)
        {
            world->clearBlocks();
            world->setChunkSize(UI.getState()->chunkSize);
            world->setChunkCount(UI.getState()->chunkCount);
            world->setShouldDrawBorderBlocks(UI.getState()->bShouldDrawChunkBorderBlocks);
            world->updateActiveChunks();
            // Very messy, just for demo
            const auto worldSize = world->getWorldSize();
            animatedHm.setMaxHeight(worldSize.y);
        }

        if (UI.getState()->bShouldGenerateHeightMap)
        {
            const auto worldSize = world->getWorldSize();
            VSHeightmap hm = VSHeightmap(42, worldSize.y, 1, 0.02F, 4.F);
            for (int x = 0; x < worldSize.x; x++)
            {
                for (int z = 0; z < worldSize.z; z++)
                {
                    for (int y = 0; y < hm.getVoxelHeight(x, z); y++)
                    {
                        world->setBlock({x, y, z}, 1);
                    }
                }
            }
            world->updateActiveChunks();
        }

        // Animate voxels, not pretty, will refactor
        if (UI.getState()->bShouldAnimateHeightMap && animatedHm.animateStep())
        {
            world->clearBlocks();
            const auto worldSize = world->getWorldSize();
            for (int x = 0; x < worldSize.x; x++)
            {
                for (int z = 0; z < worldSize.z; z++)
                {
                    for (int y = 0; y < animatedHm.getVoxelHeight(x, z); y++)
                    {
                        world->setBlock({x, y, z}, 1);
                    }
                }
            }
            world->updateActiveChunks();
        }

        if (UI.getState()->bShouldTestSetBlock)
        {
            const auto worldSize = world->getWorldSize();
            for (int x = 0; x < worldSize.x; x++)
            {
                for (int z = 0; z < worldSize.z; z++)
                {
                    for (int y = 0; y < worldSize.y - 1; y++)
                    {
                        world->setBlock({x, y, z}, 1);
                    }
                }
            }
            world->updateActiveChunks();
        }

        // Update uistate with worldState
        UI.getMutableState()->totalBlockCount = world->getTotalBlockCount();
        UI.getMutableState()->activeBlockCount = world->getActiveBlockCount();

        // draw world
        world->draw(world, nullptr);

        // draw ui
        UI.draw();
        glfwSwapBuffers(window);
    }

    // Cleanup
    UI.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
