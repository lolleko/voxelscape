#include "core/vs_game.h"

#include "core/vs_app.h"
#include "core/vs_cameracontroller.h"

#include "ui/vs_ui.h"
#include "ui/vs_ui_state.h"

#include "world/vs_world.h"

#include "world/generator/vs_heightmap.h"

void VSGame::initialize(VSApp* inApp) {
    app = inApp;
}

void VSGame::gameLoop() {
    while (!bShouldQuit) {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        auto* UI = app->getUI();
        auto* world = app->getWorld();

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

        // TODO dont pass window as param make abstract input more
        // to make thread separation clearer
        world->getCameraController()->processKeyboardInput(app->getWindow(), deltaTime);
    }
}

void VSGame::quit()
{
    bShouldQuit = true;
}
