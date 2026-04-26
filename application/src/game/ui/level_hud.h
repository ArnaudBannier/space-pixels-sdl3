/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "game/ui/level_ui_manager.h"
#include "common/common.h"

typedef struct GameContext GameContext;

typedef struct LevelHUD
{
    GameContext* m_context;
    LevelUIManager* m_uiManager;

    UIObject* m_mainPanel;
    UILabel* m_lifeLabel[MAX_PLAYER_COUNT];
} LevelHUD;

LevelHUD* LevelHUD_create(GameContext* context, LevelUIManager* manager);
void LevelHUD_destroy(LevelHUD* self);
void LevelHUD_update(LevelHUD* self);
