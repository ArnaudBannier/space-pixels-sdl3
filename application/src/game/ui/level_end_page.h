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

typedef struct LevelEndPage
{
    GameContext* m_context;
    LevelUIManager* m_uiManager;
    UIFocusManager* m_focusManager;
    UIObject* m_mainPanel;
} LevelEndPage;

LevelEndPage* LevelEndPage_create(GameContext* context, LevelUIManager* manager);
void LevelEndPage_destroy(LevelEndPage* self);
void LevelEndPage_update(LevelEndPage* self);
