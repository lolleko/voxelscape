#pragma once
#include "world/vs_chunk_manager.h"
#include "world/vs_world.h"

class VSInputHandler;

namespace VSEditor
{
    const std::string WorldName = "EDITOR";

    [[nodiscard]] VSWorld* initWorld();

    void setPlaneBlocks(VSWorld* editorWorld);

    void handleBlockPlacement(VSInputHandler* inputHandler, VSWorld* world);

    VSChunkManager::VSBuildingData extractBuildFromPlane(VSWorld* editorWorld);
};  // namespace VSEditor
