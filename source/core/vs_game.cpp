#include "core/vs_game.h"

#include <ratio>

#include "core/vs_app.h"
#include "core/vs_camera.h"
#include "core/vs_cameracontroller.h"
#include "core/vs_input_handler.h"

#include "core/vs_rtscameracontroller.h"
#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"
#include "ui/vs_parser.h"

#include "world/vs_skybox.h"
#include "world/vs_world.h"
#include "world/vs_chunk_manager.h"

#include "world/generator/vs_terrain.h"

void VSGame::initialize(VSApp* inApp)
{
    app = inApp;
    frameTimeTracker.limitFps(120);

    initializeGame(inApp);
}

void VSGame::initializeGame(VSApp* inApp)
{
    (void)inApp;
}

void VSGame::gameLoop()
{
    while (!bShouldQuit)
    {
        frameTimeTracker.startFrame();

        const auto deltaSeconds = frameTimeTracker.getDeltaSeconds();

        updateInternal(deltaSeconds);

        update(deltaSeconds);

        frameTimeTracker.endFrame();
    }
}

void VSGame::updateInternal(float /* deltaSeconds */)
{
    // TODO WHAT??? why is this here
    // PLEASE STOP THE HACKS :(
    // if (!UI->getState()->bFileBrowserActive)
    //{
    app->getInputHandler()->processKeyboardInput(
        app->getWindow(), frameTimeTracker.getDeltaSeconds());
    //}
}

void VSGame::update(float /* deltaSeconds */)
{
}

void VSGame::renderUI()
{
}

void VSGame::handleEditor()
{
    // TODO: implement to have atleast a little seperation from standard game loop
}

void VSGame::quit()
{
    bShouldQuit = true;
}

VSApp* VSGame::getApp()
{
    return app;
}