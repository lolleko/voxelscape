#include "core/vs_app.h"

#include <glad/glad.h>  // Initialize with gladLoadGL()
// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include <spdlog/common.h>
#include <thread>

#include "core/vs_core.h"
#include "core/vs_log.h"
#include "core/vs_cameracontroller.h"
#include "core/vs_camera.h"
#include "core/vs_game.h"

#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"
#include "world/vs_world.h"
#include "world/vs_skybox.h"

VSApp::VSApp()
{
}

int VSApp::initialize()
{
    debug_setMainThread();

    const auto glfwError = initializeGLFW();
    if (glfwError != 0) {
        return glfwError;
    }

    UI = new VSUI();
    // initialize logger
    VSLog::init(UI->getMutableState()->logStream);
    VSLog::Log(VSLog::Category::Core, VSLog::Level::info, "Successfully initialized logger");

    world = new VSWorld();

    VSLog::Log(
        VSLog::Category::Core,
        VSLog::Level::info,
        "Successfully setup GLFW window and opengl context");

    bool err = gladLoadGL() == 0;
    if (err)
    {
        VSLog::Log(
            VSLog::Category::Core, VSLog::Level::critical, "Failed to initialize OpenGL loader");
        return 1;
    }
    VSLog::Log(VSLog::Category::Core, VSLog::Level::info, "Successfully initialized OpenGL loader");

    // Setup Dear ImGui context
    UI->setup(glslVersion.c_str(), window);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    // auto monkeyModel = std::make_shared<VSModel>("monkey.obj");
    // auto monkeyShader = std::make_shared<VSShader>("Monkey");

    auto skybox = new VSSkybox();
    auto skyboxShader = std::make_shared<VSShader>("Skybox");

    world->initializeChunks();

    world->addDrawable(skybox, skyboxShader);

    VSLog::Log(VSLog::Category::Core, VSLog::Level::info, "Successfully initialized logger");

    game = new VSGame();
    game->initialize(this);

    VSLog::Log(VSLog::Category::Core, VSLog::Level::info, "Successfully initialized game thread");

    return 0;
}

int VSApp::initializeGLFW()
{
    // Setup window
    glfwSetErrorCallback([](int error, const char* description) {
        std::cerr << "Glfw Error " << error << ": " << description << "\n";
    });

    if (glfwInit() == 0)
    {
        return 1;
    }

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    glslVersion = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    glslVersion = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // 3.0+ only
#endif

    // Create window with graphics context
    const auto width = 1280;
    const auto height = 720;

    window = glfwCreateWindow(width, height, "Voxelscape", nullptr, nullptr);
    if (window == nullptr)
    {
        VSLog::Log(VSLog::Category::Core, VSLog::Level::critical, "Failed to create GLFW window");
        return 1;
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, (void*)this);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        auto* app = static_cast<VSApp*>(glfwGetWindowUserPointer(window));
        app->getWorld()->getCameraController()->processFramebufferResize(window, width, height);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        auto* app = static_cast<VSApp*>(glfwGetWindowUserPointer(window));
        app->getWorld()->getCameraController()->processMouseMovement(window, xpos, ypos);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        auto* app = static_cast<VSApp*>(glfwGetWindowUserPointer(window));
        app->getWorld()->getCameraController()->processMouseButton(window, button, action, mods);
    });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        auto* app = static_cast<VSApp*>(glfwGetWindowUserPointer(window));
        app->getWorld()->getCameraController()->processMouseScroll(window, xoffset, yoffset);
    });
    glfwSwapInterval(1);

    return 0;
}

VSWorld* VSApp::getWorld()
{
    return world;
}

VSUI* VSApp::getUI()
{
    return UI;
}

GLFWwindow* VSApp::getWindow()
{
    return window;
}

int VSApp::mainLoop()
{
    // start game loop
    std::thread gameThread(&VSGame::gameLoop, game);

    // Main loop
    while (glfwWindowShouldClose(window) == 0)
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        UI->render();

        // TODO one ui for game one for rendering debug
        if (UI->getState()->isWireframeModeEnabled)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        UI->getMutableState()->totalBlockCount = world->getTotalBlockCount();
        UI->getMutableState()->activeBlockCount = world->getActiveBlockCount();

        auto display_w = 0;
        auto display_h = 0;

        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        const auto clearColor = UI->getState()->clearColor;
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

        // Clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update chunks
        world->updateActiveChunks();

        // draw world
        world->draw(world, nullptr);

        // draw ui
        UI->draw();

        glfwSwapBuffers(window);
    }

    // Shut down game thread
    game->quit();

    gameThread.join();

    // Cleanup
    UI->cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}