#include "core/vs_game.h"

#include "core/vs_app.h"
#include "core/vs_cameracontroller.h"

#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"

#include "world/vs_world.h"

#include "world/generator/vs_heightmap.h"

// TODO: Remove debugging includes
#include <iostream>
#include "core/vs_camera.h"

void VSGame::initialize(VSApp* inApp)
{
    app = inApp;
}

void VSGame::gameLoop()
{
    while (!bShouldQuit)
    {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

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
            world->clearBlocks();
            world->setChunkDimensions(UI->getState()->chunkSize, UI->getState()->chunkCount);
            UI->getMutableState()->bShouldUpdateChunks = false;
        }

        if (UI->getState()->bShouldGenerateHeightMap)
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
            UI->getMutableState()->bShouldGenerateHeightMap = false;
        }

        if (UI->getState()->bShouldResetEditor)
        {
            std::cout << "Camera pitch " << world->getCamera()->getPitch() << std::endl;
            std::cout << "Camera yaw " << world->getCamera()->getYaw() << std::endl;
            std::cout << "Camera Position " << (world->getCamera()->getPosition()).x << ", " << (world->getCamera()->getPosition()).y << ", " << (world->getCamera()->getPosition()).z << std::endl;
            world->clearBlocks();
            const auto worldSize = world->getWorldSize();
            for (int x = 0; x < worldSize.x; x++)
            {
                for (int z = 0; z < worldSize.z; z++)
                {
                    world->setBlock({x, 0, z}, 1);
                }
            }
            UI->getMutableState()->bShouldResetEditor = false;
        }

        // TODO dont pass window as param make abstract input more
        // to make thread separation clearer
        world->getCameraController()->processKeyboardInput(app->getWindow(), deltaTime);
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
