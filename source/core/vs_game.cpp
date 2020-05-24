#include "core/vs_game.h"

#include <ratio>

#include "core/vs_app.h"
#include "core/vs_cameracontroller.h"

#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"

#include "world/vs_world.h"
#include "world/vs_chunk_manager.h"

#include "world/generator/vs_heightmap.h"

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
        auto* world = app->getWorld();

        // Update world state with ui state
        if (UI->getState()->bShouldUpdateChunks)
        {
            world->getChunkManager()->setChunkDimensions(
                UI->getState()->chunkSize, UI->getState()->chunkCount);
            UI->getMutableState()->bShouldUpdateChunks = false;
        }

        if (UI->getState()->bShouldGenerateHeightMap)
        {
            const auto worldSize = world->getChunkManager()->getWorldSize();
            VSHeightmap hm = VSHeightmap(42, worldSize.y, 1, 0.02F, 4.F);
            for (int x = 0; x < worldSize.x; x++)
            {
                for (int z = 0; z < worldSize.z; z++)
                {
                    for (int y = 0; y < hm.getVoxelHeight(x, z); y++)
                    {
                        if (y > worldSize.y / 2)
                        {
                            world->getChunkManager()->setBlock({x, y, z}, 1);
                        }
                        else
                        {
                            world->getChunkManager()->setBlock({x, y, z}, 2);
                        }
                    }
                }
            }
            UI->getMutableState()->bShouldGenerateHeightMap = false;
        }

        // TODO dont pass window as param make abstract input more
        // to make thread separation clearer
        world->getCameraController()->processKeyboardInput(
            app->getWindow(), frameTimeTracker.getDeltaSeconds());

        frameTimeTracker.endFrame();
    }
}

void VSGame::quit()
{
    bShouldQuit = true;
}
