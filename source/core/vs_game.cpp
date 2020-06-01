#include "core/vs_game.h"

#include <ratio>

#include "core/vs_app.h"
#include "core/vs_cameracontroller.h"

#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"
#include "ui/vs_parser.h"

#include "world/vs_world.h"
#include "world/vs_chunk_manager.h"

#include "world/generator/vs_terrain.h"

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
            app->setEditorWorldActive();
            UI->getMutableState()->bShouldSetEditorActive = false;
        }
        if (UI->getState()->bShouldSetGameActive)
        {
            app->setGameWorldActive();
            UI->getMutableState()->bShouldSetGameActive = false;
        }
        auto* world = app->getActiveWorld();

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

        if (UI->getState()->bShouldLoadFromFile)
        {
            VSChunkManager::VSWorldData worldData =
                VSParser::readFromFile(UI->getState()->loadFilePath);
            world->getChunkManager()->setWorldData(worldData);
            UI->getMutableState()->bShouldLoadFromFile = false;
        }

        if (UI->getState()->bShouldGenerateTerrain)
        {
            if (UI->getState()->bBiomeType == 0)
            {
                VSTerrainGeneration::buildTaiga(world);
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
            const auto worldSize = world->getChunkManager()->getWorldSize();
            for (int x = 0; x < worldSize.x; x++)
            {
                for (int z = 0; z < worldSize.z; z++)
                {
                    world->getChunkManager()->setBlock({x, 0, z}, 1);
                }
            }
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
