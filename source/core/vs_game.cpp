#include "core/vs_game.h"

#include <ratio>

#include "core/vs_app.h"
#include "core/vs_editor.h"
#include "core/vs_cameracontroller.h"

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
}

void VSGame::gameLoop()
{
    while (!bShouldQuit)
    {
        frameTimeTracker.startFrame();

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
            VSChunkManager::VSBuildingData buildData = VSParser::readBuildFromFile(UI->getState()->loadFilePath);
            (void) buildData;
        }

        if (UI->getState()->bShouldGenerateTerrain && !world->getChunkManager()->shouldReinitializeChunks())
        {
            if (UI->getState()->bBiomeType == 0)
            {
                VSTerrainGeneration::buildMountains(world);
            }
            else if (UI->getState()->bBiomeType == 1)
            {
                VSTerrainGeneration::buildDesert(world);
            }
            UI->getMutableState()->bShouldGenerateTerrain = false;
        }

        if (UI->getState()->bShouldResetEditor &&
            !world->getChunkManager()->shouldReinitializeChunks())
        {
            // TODO: Clear world
            // world->getChunkManager()->clearBlocks();
            VSEditor::setPlaneBlocks(world);
            UI->getMutableState()->bShouldResetEditor = false;
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
            world->getCameraController()->processKeyboardInput(
                app->getWindow(), frameTimeTracker.getDeltaSeconds());
        }

        frameTimeTracker.endFrame();
    }
}

void VSGame::handleEditor()
{
    // TODO: implement to have atleast a little seperation from standard game loop
}

void VSGame::quit()
{
    bShouldQuit = true;
}

VSWorld* VSGame::initWorld()
{
    VSWorld* gameWorld = new VSWorld();
    auto skybox = new VSSkybox();
    gameWorld->addDrawable(skybox);
    return gameWorld;
}