#include "game/systems/menu_system.h"
#include <entt/entity/entity.hpp>
#include "core/vs_camera.h"
#include "game/components/inputs.h"
#include "game/components/ui_context.h"
#include "game/components/world_context.h"
#include "world/generator/vs_terrain.h"
#include "game/systems/upgrade_system.h"
#include "game/systems/delete_system.h"
#include "core/vs_app.h"

void updateMenuSystem(entt::registry& mainRegistry, entt::registry& buildingRegistry)
{
    auto& uiContext = mainRegistry.ctx().get<UIContext>();
    auto& inputContext = mainRegistry.ctx().get<Inputs>();
    auto& worldContext = mainRegistry.ctx().get<WorldContext>();

    auto* app = VSApp::getInstance();

    if (uiContext.bShouldSetEditorActive)
    {
        app->setWorldActive(uiContext.editorWorldName);
        uiContext.bShouldUpdateChunks = true;
        uiContext.bShouldResetEditor = true;
        uiContext.bEditorActive = true;
        uiContext.bMenuActive = false;
        worldContext.world = app->getWorld();
        uiContext.bShouldSetEditorActive = false;
    }
    if (uiContext.bShouldSetGameActive)
    {
        app->setWorldActive(uiContext.gameWorldName);
        uiContext.bGameConfigActive = false;
        uiContext.bMenuActive = false;
        worldContext.world = app->getWorld();
        uiContext.bShouldSetGameActive = false;
    }
    if (uiContext.bShouldStartGame)
    {
        app->setWorldActive(uiContext.gameWorldName);
        uiContext.bShouldUpdateChunks = true;
        uiContext.bShouldGenerateTerrain = true;
        uiContext.bGameConfigActive = false;
        uiContext.bMenuActive = false;
        worldContext.world = app->getWorld();
        uiContext.bShouldStartGame = false;
        uiContext.bIsGameWorldRunning = true;
    }
    if ((inputContext.JustUp & VSInputHandler::KEY_ESCAPE) != 0 &&
        uiContext.selectedBuilding.uuid.empty())
    {
        app->setWorldActive(uiContext.menuWorldName);
        uiContext.bGameConfigActive = false;
        uiContext.bMenuActive = true;
        uiContext.bEditorActive = false;
        uiContext.bShouldSetGameActive = false;
        worldContext.world = app->getWorld();
    }
    auto* world = worldContext.world;

    if (app->getWorldName() == uiContext.menuWorldName)
    {
        if (uiContext.bIsMenuWorldInitialized &&
            !world->getChunkManager()->shouldReinitializeChunks())
        {
            // Rotate camera while in menu
            world->getCamera()->setPitchYaw(
                0.F, world->getCamera()->getYaw() + worldContext.deltaSeconds * 3.F);

            const auto groundTraceStart = glm::vec3(0.F, 200.F, 0.F);
            const auto groundTraceResult = world->getChunkManager()->lineTrace(
                groundTraceStart, groundTraceStart - glm::vec3(0.F, 500.F, 0.F));

            const auto collisionFrontStart =
                groundTraceResult.hitLocation + groundTraceResult.hitNormal + 8.F;

            const auto collisionFrontTraceResult = world->getChunkManager()->lineTrace(
                collisionFrontStart, collisionFrontStart + world->getCamera()->getFront() * 6.f);

            auto cameraTargetPos = collisionFrontStart;

            if (collisionFrontTraceResult.bHasHit)
            {
                const auto collisionBackStart = collisionFrontTraceResult.hitLocation;

                const auto collisionBackEnd =
                    collisionBackStart - world->getCamera()->getFront() * 12.F;

                const auto collisionBackTraceResult =
                    world->getChunkManager()->lineTrace(collisionBackStart, collisionBackEnd);

                if (collisionBackTraceResult.bHasHit)
                {
                    cameraTargetPos = collisionBackTraceResult.hitLocation +
                                      world->getCamera()->getFront() * 0.1F;
                }
                else
                {
                    cameraTargetPos = collisionBackEnd;
                }
            }

            world->getCamera()->setPosition(glm::mix(
                world->getCamera()->getPosition(),
                cameraTargetPos,
                0.25F * worldContext.deltaSeconds));
        }

        if (!uiContext.bIsMenuWorldInitialized)
        {
            world->getCamera()->setPosition({0.F, 100.F, 0.F});
            uiContext.bIsMenuWorldInitialized = true;
            uiContext.bShouldUpdateChunks = true;
            uiContext.bShouldGenerateTerrain = true;
        }
    }

    if (app->getWorldName() == uiContext.editorWorldName)
    {
    }

    if (app->getWorldName() == uiContext.gameWorldName)
    {
        uiContext.bShowBuildingWindow = (uiContext.selectedBuildingEntity != entt::null);

        if (uiContext.bDestroyBuildingEntity)
        {
            if (uiContext.selectedBuildingEntity != entt::null)
            {
                deleteSelectedBuilding(mainRegistry, buildingRegistry);
            }

            uiContext.selectedBuildingEntity = entt::null;
            uiContext.bDestroyBuildingEntity = false;
        }

        if (uiContext.bUpgradeBuildingEntity)
        {
            upgradeBuilding(mainRegistry, buildingRegistry);
            uiContext.bUpgradeBuildingEntity = false;
        }
    }

    // Update world state with ui state
    if (uiContext.bShouldUpdateChunks)
    {
        glm::ivec2 chunkCount = {4, 4};

        if (uiContext.worldSize == 0)
        {
            // Small
            chunkCount = {16, 16};
        }
        else if (uiContext.worldSize == 1)
        {
            // Medium
            chunkCount = {32, 32};
        }
        else if (uiContext.worldSize == 2)
        {
            // Large
            chunkCount = {64, 64};
        }
        else if (uiContext.worldSize == 3)
        {
            // Tiny for debug
            chunkCount = {2, 2};
        }

        if (uiContext.bEditorActive)
        {
            chunkCount = {4, 4};
        }

        constexpr glm::ivec3 chunkSize = {32, 128, 32};

        world->getChunkManager()->setChunkDimensions(chunkSize, chunkCount);
        uiContext.bShouldUpdateChunks = false;
    }

    if (uiContext.bShouldGenerateTerrain && !world->getChunkManager()->shouldReinitializeChunks())
    {
        uiContext.bShouldGenerateTerrain = false;

        uiContext.bShowLoading = true;
        if (uiContext.selectedBiomeType == 0)
        {
            VSTerrainGeneration::buildStandard(world);
        }
        else if (uiContext.selectedBiomeType == 1)
        {
            VSTerrainGeneration::buildMountains(world);
        }
        else if (uiContext.selectedBiomeType == 2)
        {
            VSTerrainGeneration::buildDesert(world);
        }
        uiContext.bShowLoading = false;

        if (!uiContext.bEditorActive)
        {
            uiContext.minimap.bShouldUpdate = true;
        }
    }

    if (uiContext.bShouldResetEditor && !world->getChunkManager()->shouldReinitializeChunks())
    {
        VSTerrainGeneration::buildEditorPlane(world);
        uiContext.bShouldResetEditor = false;
    }
}