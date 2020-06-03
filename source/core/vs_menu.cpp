#include "core/vs_menu.h"
#include "core/vs_dummycameracontroller.h"
#include "world/vs_skybox.h"
#include "world/vs_world.h"

namespace VSMenu
{
    VSWorld* initWorld()
    {
        VSWorld* menuWorld = new VSWorld();
        auto skybox = new VSSkybox();
        menuWorld->addDrawable(skybox);
        auto dummyController = new VSDummyCameraController();
        menuWorld->setCameraController(dummyController);
        return menuWorld;
    }
}  // namespace VSMenu