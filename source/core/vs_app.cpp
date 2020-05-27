#include "core/vs_app.h"

#include <glad/glad.h>  // Initialize with gladLoadGL()
// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include <spdlog/common.h>
#include <thread>

#include "core/vs_core.h"
#include "core/vs_log.h"
#include "core/vs_cameracontroller.h"
// TODO: Remove and find better position to set camera cantroller
#include "core/vs_rtscameracontroller.h"
#include "core/vs_dummycameracontroller.h"
#include "core/vs_camera.h"
#include "core/vs_game.h"
#include "core/vs_debug_draw.h"

#include "world/vs_chunk_manager.h"

#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"
#include "world/vs_chunk_manager.h"
#include "world/vs_world.h"
#include "world/vs_skybox.h"

VSApp* VSApp::instance = nullptr;

VSApp::VSApp()
{
    // singleton dont construct more than once
    assert(!VSApp::instance);
    if (!VSApp::instance)
    {
        VSApp::instance = this;
    }
}

int VSApp::initialize()
{
    debug_setMainThread();

    const auto glfwError = initializeGLFW();
    if (glfwError != 0)
    {
        return glfwError;
    }

    UI = new VSUI();
    // initialize logger
    VSLog::init(UI->getMutableState()->logStream);
    VSLog::Log(VSLog::Category::Core, VSLog::Level::info, "Successfully initialized logger");

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

    world = new VSWorld();
    editorWorld = new VSWorld();
    menuWorld = new VSWorld();
    auto dummyController = new VSDummyCameraController();
    menuWorld->setCameraController(dummyController);

    auto skybox = new VSSkybox();
    world->addDrawable(skybox);
    editorWorld->addDrawable(skybox);
    menuWorld->addDrawable(skybox);

    // TODO: initialize editor world method, maybe in VSWorld?
    const auto worldSize = editorWorld->getChunkManager()->getWorldSize();
    for (int x = 0; x < worldSize.x; x++)
    {
        for (int z = 0; z < worldSize.z; z++)
        {
            editorWorld->getChunkManager()->setBlock({x, 0, z}, 1);
        }
    }
    editorWorld->getCamera()->setPosition(glm::vec3(-50.F, -5.F, -50.F));
    editorWorld->getCamera()->setPitchYaw(-10.F, 45.F);

    // TODO: initialize menu world somewhere else, maybe in VSWorld?
    // TODO: initiialize menu world

    // Set menu world active initially
    activeWorld = menuWorld;

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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // 3.0+ only
#endif

    // anti Aliasing
    glfwWindowHint(GLFW_SAMPLES, 8);

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
        app->getActiveWorld()->getCameraController()->processFramebufferResize(
            window, width, height);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        auto* app = static_cast<VSApp*>(glfwGetWindowUserPointer(window));
        app->getActiveWorld()->getCameraController()->processMouseMovement(window, xpos, ypos);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
        auto* app = static_cast<VSApp*>(glfwGetWindowUserPointer(window));
        app->getActiveWorld()->getCameraController()->processMouseButton(
            window, button, action, mods);
    });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        auto* app = static_cast<VSApp*>(glfwGetWindowUserPointer(window));
        app->getActiveWorld()->getCameraController()->processMouseScroll(window, xoffset, yoffset);
    });
    glfwSwapInterval(1);
    // Capture cursor for FPS Camera
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return 0;
}

VSWorld* VSApp::getWorld() const
{
    return world;
}

VSWorld* VSApp::getEditorWorld()
{
    return editorWorld;
}

void VSApp::setEditorWorldActive()
{
    activeWorld = editorWorld;
}

void VSApp::setGameWorldActive()
{
    activeWorld = world;
}

VSWorld* VSApp::getActiveWorld()
{
    return activeWorld;
}

VSUI* VSApp::getUI() const
{
    return UI;
}

GLFWwindow* VSApp::getWindow() const
{
    return window;
}

VSApp* VSApp::getInstance()
{
    return instance;
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

        UI->getMutableState()->totalBlockCount = activeWorld->getChunkManager()->getTotalBlockCount();
        UI->getMutableState()->visibleBlockCount = activeWorld->getChunkManager()->getVisibleBlockCount();
        UI->getMutableState()->drawnBlockCount = activeWorld->getChunkManager()->getDrawnBlockCount();
        UI->getMutableState()->drawCallCount = activeWorld->getChunkManager()->getDrawCallCount();

        world->setDirectLightPos(UI->getState()->directLightPos);

        // TODO add option for day night
        //world->setDirectLightPos(glm::vec3(world->getChunkManager()->getWorldSize() * 2) * glm::vec3(cos(glfwGetTime() / 10.f),  sin(glfwGetTime() / 10.f), 0.f));

        world->getDebugDraw()->drawSphere(world->getDirectLightPos(), 10.f, {255, 255, 255});

        auto display_w = 0;
        auto display_h = 0;

        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        const auto clearColor = UI->getState()->clearColor;
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);

        // Clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update chunks
        activeWorld->update();

        // draw world
        activeWorld->draw(activeWorld);

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