#include "core/vs_editor.h"
#include "core/vs_camera.h"
#include "world/vs_chunk_manager.h"
#include "world/vs_skybox.h"
#include "world/vs_world.h"

namespace VSEditor
{
    VSWorld* initWorld()
    {
        VSWorld* editorWorld = new VSWorld();
        auto skybox = new VSSkybox();
        editorWorld->addDrawable(skybox);
        editorWorld->getCamera()->setPosition(glm::vec3(-50.F, -5.F, -50.F));
        editorWorld->getCamera()->setPitchYaw(-10.F, 45.F);
        return editorWorld;
    }

    // Set blocks for a plane, should be called with the world as parameter that was returned by initWorld() after the chunks were initialized
    void setPlaneBlocks(VSWorld* editorWorld)
    {
        const auto worldSize = editorWorld->getChunkManager()->getWorldSize();
        for (int x = 0; x < worldSize.x; x++)
        {
            for (int z = 0; z < worldSize.z; z++)
            {
                editorWorld->getChunkManager()->setBlock({x, 0, z}, 1);
            }
        }
    }
};  // namespace VSEditor