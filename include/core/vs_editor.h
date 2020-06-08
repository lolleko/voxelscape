#pragma once
#include "world/vs_chunk_manager.h"
#include "world/vs_world.h"

namespace VSEditor
{
    const std::string WorldName = "EDITOR";

    [[nodiscard]] VSWorld* initWorld();

    void setPlaneBlocks(VSWorld* editorWorld);

    VSChunkManager::VSBuildingData extractBuildFromPlane(VSWorld* editorWorld);
};  // namespace VSEditor
