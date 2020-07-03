#pragma once

#include <entt/entt.hpp>
#include "core/vs_app.h"
#include "core/vs_game.h"
#include "game/components/ui_context.h"

class Voxelscape : public VSGame
{
public:
    void initializeGame(VSApp* inApp) override;

    void update(float deltaSeconds) override;

    void renderUI() override;

private:
    entt::registry mainRegistry;
    entt::registry buildingRegistry;

    void renderEditorGUI(UIContext& uiState);
    void renderMainMenu(UIContext& uiState);
    void renderGameGUI(UIContext& uiState);
    void renderGameConfigGUI(UIContext& uiState);
    void renderLoading(UIContext& uiState);
};