#include "core/vs_game.h"

#include <ratio>

#include "core/vs_app.h"
#include "core/vs_camera.h"
#include "core/vs_editor.h"
#include "core/vs_cameracontroller.h"
#include "core/vs_input_handler.h"

#include "core/vs_rtscameracontroller.h"
#include "ui/vs_minimap.h"
#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"
#include "ui/vs_parser.h"

#include "world/vs_skybox.h"
#include "world/vs_world.h"
#include "world/vs_chunk_manager.h"

#include "world/generator/vs_terrain.h"

const std::string VSGame::WorldName = "GAME";

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

void VSGame::updateInternal(float deltaSeconds)
{
    (void)deltaSeconds;

    auto* UI = app->getUI();

    if (UI->getState()->bShouldSetEditorActive)
    {
        app->setWorldActive(VSEditor::WorldName);
        UI->getMutableState()->bShouldSetEditorActive = false;
    }
    if (UI->getState()->bShouldSetGameActive)
    {
        app->setWorldActive(WorldName);
        UI->getMutableState()->bShouldSetGameActive = false;
    }

    auto* world = app->getWorld();

    // Update world state with ui state
    if (UI->getState()->bShouldUpdateChunks)
    {
        world->getChunkManager()->setChunkDimensions(
            UI->getState()->chunkSize, UI->getState()->chunkCount);
        UI->getMutableState()->bShouldUpdateChunks = false;
    }

    if (UI->getState()->bShouldSaveToFile)
    {
        VSChunkManager::VSWorldData worldData = world->getChunkManager()->getData();
        VSParser::writeToFile(worldData, UI->getState()->saveFilePath);
        UI->getMutableState()->bShouldSaveToFile = false;
    }

    if (UI->getState()->bShouldSaveBuilding)
    {
        // Extract editor plane and save building
        VSChunkManager::VSBuildingData buildData = VSEditor::extractBuildFromPlane(world);
        VSParser::writeBuildToFile(buildData, UI->getState()->saveBuildingPath);
        UI->getMutableState()->bShouldSaveBuilding = false;
    }

    if (UI->getState()->bShouldLoadFromFile)
    {
        VSChunkManager::VSWorldData worldData =
            VSParser::readFromFile(UI->getState()->loadFilePath);
        world->getChunkManager()->setWorldData(worldData);
        UI->getMutableState()->bShouldLoadFromFile = false;
    }

    if (UI->getState()->bShouldGenerateTerrain &&
        !world->getChunkManager()->shouldReinitializeChunks())
    {
        if (UI->getState()->bBiomeType == 0)
        {
            VSTerrainGeneration::buildStandard(world);
        }
        else if (UI->getState()->bBiomeType == 1)
        {
            VSTerrainGeneration::buildMountains(world);
        }
        else if (UI->getState()->bBiomeType == 2)
        {
            VSTerrainGeneration::buildDesert(world);
        }
        UI->getMutableState()->bShouldGenerateTerrain = false;

        if (!UI->getState()->bEditorActive)
        {
            // Update Minimap
            UI->getMutableState()->minimap->updateMinimap(world);
        }
    }

    if (UI->getState()->bShouldResetEditor && !world->getChunkManager()->shouldReinitializeChunks())
    {
        // TODO: Clear world
        // world->getChunkManager()->clearBlocks();
        VSEditor::setPlaneBlocks(world);
        UI->getMutableState()->bShouldResetEditor = false;
    }

    if (UI->getState()->bEditorActive)
    {
        VSEditor::handleBlockPlacement(
            app->getInputHandler(), world, UI->getState()->anyWindowHovered);
    }

    if (UI->getState()->bShouldUpdateBlockID)
    {
        // Increment because list starts at 0 and we do not want to set "Air" blocks
        world->getCameraController()->setEditorBlockID(UI->getState()->bSetBlockID + 1);
        UI->getMutableState()->bShouldUpdateBlockID = false;
    }

    // TODO dont pass window as param make abstract input more
    // to make thread separation clearer
    if (!UI->getState()->bFileBrowserActive)
    {
        app->getInputHandler()->processKeyboardInput(
            app->getWindow(), frameTimeTracker.getDeltaSeconds());
    }
}

void VSGame::update(float deltaSeconds)
{
    (void)deltaSeconds;
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

VSWorld* VSGame::initWorld()
{
    VSWorld* gameWorld = new VSWorld();
    auto camera = gameWorld->getCamera();
    // camera->setPosition({0.F, 50.F, 0.F});
    auto skybox = new VSSkybox();
    auto cameraController = new VSRTSCameraController(camera, gameWorld);
    gameWorld->setCameraController(cameraController);
    gameWorld->addDrawable(skybox);
    return gameWorld;
}