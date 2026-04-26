/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/ui/level_hud.h"
#include "game/ui/title_ui_manager.h"
#include "game/core/game_context.h"
#include "common/ui_style.h"
#include "game/game_config.h"

LevelHUD* LevelHUD_create(GameContext* context, LevelUIManager* manager)
{
    AssetManager* assets = GameContext_getAssetManager(context);
    UISystem* uiSystem = GameContext_getUISystem(context);
    UIObject* canvas = UISystem_getCanvas(uiSystem);
    UIRect rect = { 0 };
    RenderSortingLayer sortingLayer = { 0 };
    TTF_Font* font = NULL;
    UILabel* label = NULL;

    Input_clearUIInput(GameContext_getInput(context));

    LevelHUD* self = (LevelHUD*)calloc(1, sizeof(LevelHUD));
    ASSERT_NEW(self);

    self->m_uiManager = manager;
    self->m_context = context;

    self->m_mainPanel = UIObject_create(uiSystem, "hud-panel");
    UIObject_setParent(self->m_mainPanel, canvas);

    const int playerCount = g_gameConfig.playerCount;
    UIGridLayout* hudLayout = UIGridLayout_create(uiSystem, "player-info-layout", 1, playerCount);
    UIObject_setParent(hudLayout, self->m_mainPanel);
    UIGridLayout_setRowSizes(hudLayout, 30.0f);
    UIGridLayout_setColumnSizes(hudLayout, 60.0f);
    UIGridLayout_setColumnSpacings(hudLayout, 10.f);
    UIGridLayout_setAnchor(hudLayout, Vec2_anchor_north_east);
    UIGridLayout_setPadding(hudLayout, Vec2_set(20.f, 20.f));

    SpriteSheet* uiSpriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_UI_BASE);
    SpriteGroup* spriteGroup = SpriteSheet_getGroupByName(uiSpriteSheet, "button");

    for (int i = 0; i < playerCount; i++)
    {
        font = AssetManager_getFont(assets, FONT_HUD_VALUE);
        label = UILabel_create(uiSystem, "life-label", font);
        self->m_lifeLabel[i] = label;
        UILabel_setTextString(label, "100");
        UILabel_setAnchor(label, Vec2_anchor_center);
        UILabel_setColor(label, g_colors.white);
        UIGridLayout_addObject(hudLayout, label, 0, i, 1, 1);
    }

    return self;
}

void LevelHUD_destroy(LevelHUD* self)
{
    if (!self) return;

    UIObject_destroy(self->m_mainPanel);

    free(self);
}

void LevelHUD_update(LevelHUD* self)
{
    for (int i = 0; i < g_gameConfig.playerCount; i++)
    {
        int life = (int)roundf(g_gameConfig.playerHPs[i]);
        int score = (int)roundf(g_gameConfig.playerScores[i]);
        char buffer[64] = { 0 };
        sprintf(buffer, "%d", life);
        UILabel_setTextString(self->m_lifeLabel[i], buffer);
    }
}
