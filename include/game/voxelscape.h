#pragma once

#include <entt/entt.hpp>
#include "core/vs_app.h"
#include "core/vs_game.h"

class Voxelscape : public VSGame
{
public:
    void initializeGame(VSApp* inApp) override;

    void update(float deltaSeconds) override;

private:
    entt::registry mainRegistry;
    entt::registry buildingRegistry;
};