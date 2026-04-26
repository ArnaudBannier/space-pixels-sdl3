/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "game/ui/title_ui_manager.h"

typedef struct GameContext GameContext;

typedef struct TitleLevelMenu
{
    GameContext* m_context;
    TitleUIManager* m_uiManager;
    UIFocusManager* m_focusManager;
    UIObject* m_mainPanel;
} TitleLevelMenu;

TitleLevelMenu* TitleLevelMenu_create(GameContext* context, TitleUIManager* manager);
void TitleLevelMenu_destroy(TitleLevelMenu* self);
void TitleLevelMenu_update(TitleLevelMenu* self);
