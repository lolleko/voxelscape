#pragma once

#include <entt/entt.hpp>
#include "core/vs_debug_draw.h"
#include "game/components/inputs.h"
#include "game/components/ui_context.h"
#include "game/components/world_context.h"
#include "ui/vs_parser.h"
#include "world/vs_chunk_manager.h"

void updateEditorSystem(entt::registry& mainRegistry);

//     if (uiContext.bShouldSaveBuilding)
//     {
//         // Extract editor plane and save building
//         VSChunkManager::VSBuildingData buildData = VSEditor::extractBuildFromPlane(world);
//         VSParser::writeBuildToFile(buildData, uiContext.saveBuildingPath);
//         uiContext.bShouldSaveBuilding = false;
//     }

VSChunkManager::VSBuildingData extractBuildFromPlane(VSWorld* editorWorld);