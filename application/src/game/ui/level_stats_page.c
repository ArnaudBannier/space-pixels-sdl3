/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/ui/level_stats_page.h"
#include "game/ui/title_ui_manager.h"
#include "game/core/game_context.h"
#include "common/ui_style.h"
#include "game/game_config.h"
#include "game/core/game_engine.h"

//LevelStatsPage* LevelStatsPage_create(GameContext* context, LevelUIManager* manager)
//{
//    AssetManager* assets = GameContext_getAssetManager(context);
//    UISystem* uiSystem = GameContext_getUISystem(context);
//    UIObject* canvas = UISystem_getCanvas(uiSystem);
//    UIRect rect = { 0 };
//    RenderSortingLayer sortingLayer = { 0 };
//    TTF_Font* font = NULL;
//    UILabel* label = NULL;
//
//    Input_clearUIInput(GameContext_getInput(context));
//
//    LevelStatsPage* self = (LevelStatsPage*)calloc(1, sizeof(LevelStatsPage));
//    ASSERT_NEW(self);
//
//    self->m_uiManager = manager;
//    self->m_context = context;
//    self->m_mainPanel = UIObject_create(uiSystem, "main-panel");
//    self->m_focusManager = UIFocusManager_create(uiSystem);
//
//    UIObject_setParent(self->m_mainPanel, canvas);
//
//    // Background
//    UIFillRect* background = UIFillRect_create(uiSystem, "background", g_colors.black);
//    UIObject_setParent(background, self->m_mainPanel);
//    sortingLayer.layer = GAME_LAYER_UI;
//    sortingLayer.orderInLayer = -2;
//    UIObject_setRenderLayer(background, sortingLayer);
//    UIFillRect_setOpacity(background, 0.5f);
//
//    // Layout
//    const int playerCount = g_gameConfig.playerCount;
//    UIGridLayout* layout = UIGridLayout_create(uiSystem, "main-layout", 2, 1);
//    UIObject_setParent(layout, self->m_mainPanel);
//    UIGridLayout_setRowSizes(layout, 25.0f);
//    UIGridLayout_setRowSpacing(layout, 0, 20.f);
//
//    // Title
//    font = AssetManager_getFont(assets, FONT_TITLE);
//    label = UILabel_create(uiSystem, "title-label", font);
//    UILabel_setTextString(label, "Level Stats");
//    UILabel_setAnchor(label, Vec2_anchor_center);
//    UILabel_setColor(label, g_colors.white);
//
//    UIGridLayout_addObject(layout, label, 0, 0, 1, playerCount);
//
//    // Table Background
//    SpriteSheet* uiSpriteSheet = AssetManager_getSpriteSheet(assets, SPRITE_UI_BASE);
//    SpriteGroup* spriteGroup = SpriteSheet_getGroupByName(uiSpriteSheet, "button");
//    UIImage* tableBackground = UIImage_create(uiSystem, "table-background", spriteGroup, 0);
//    sortingLayer.layer = GAME_LAYER_UI;
//    sortingLayer.orderInLayer = -1;
//    UIObject_setRenderLayer(tableBackground, sortingLayer);
//    UIImage_setColorMod(tableBackground, g_colors.cyan5);
//    UIGridLayout_addObject(layout, tableBackground, 1, 0, 1, 1);
//
//    // Table layout
//    UIGridLayout* tableLayout = UIGridLayout_create(uiSystem, "table-layout", 2, playerCount + 1);
//    UIGridLayout_setRowSizes(tableLayout, 15.0f);
//    UIGridLayout_setRowSpacings(tableLayout, 5.f);
//    UIGridLayout_setColumnSizes(tableLayout, 50.0f);
//    UIGridLayout_setColumnSize(tableLayout, 0, 75.0f);
//    UIGridLayout_setPadding(tableLayout, Vec2_set(20.f, 10.f));
//
//    UIGridLayout_addObject(layout, tableLayout, 1, 0, 1, 1);
//
//    Vec2 tableSize = UIGridLayout_getMinimumSize(tableLayout);
//    UIGridLayout_setRowSize(layout, 1, tableSize.y);
//    UIGridLayout_setColumnSize(layout, 0, tableSize.x);
//
//    const char* labelNames[2] = { "score-label", "life-label" };
//    const char* labelStrings[2] = { "Score", "Life" };
//    font = AssetManager_getFont(assets, FONT_NORMAL);
//    for (int i = 0; i < 2; i++)
//    {
//        label = UILabel_create(uiSystem, labelNames[i], font);
//        UILabel_setTextString(label, labelStrings[i]);
//        UILabel_setAnchor(label, Vec2_anchor_west);
//        UILabel_setColor(label, g_colors.cyan4);
//        UIGridLayout_addObject(tableLayout, label, i, 0, 1, 1);
//    }
//
//    for (int i = 0; i < playerCount; i++)
//    {
//        char buffer[64] = { 0 };
//        int score = (int)roundf(g_gameConfig.playerScores[i]);
//        int life = (int)roundf(g_gameConfig.playerHPs[i]);
//
//        font = AssetManager_getFont(assets, FONT_NORMAL);
//        label = UILabel_create(uiSystem, "score-value-label", font);
//        snprintf(buffer, sizeof(buffer), "%d", score);
//        UILabel_setTextString(label, buffer);
//        UILabel_setAnchor(label, Vec2_anchor_west);
//        UILabel_setColor(label, g_colors.white);
//        UIGridLayout_addObject(tableLayout, label, 0, i + 1, 1, 1);
//
//        label = UILabel_create(uiSystem, "life-value-label", font);
//        snprintf(buffer, sizeof(buffer), "%d", life);
//        UILabel_setTextString(label, buffer);
//        UILabel_setAnchor(label, Vec2_anchor_west);
//        UILabel_setColor(label, g_colors.white);
//        UIGridLayout_addObject(tableLayout, label, 1, i + 1, 1, 1);
//    }
//
//    return self;
//}
//
//void LevelStatsPage_destroy(LevelStatsPage* self)
//{
//    if (!self) return;
//
//    UIFocusManager_destroy(self->m_focusManager);
//    UIObject_destroy(self->m_mainPanel);
//
//    free(self);
//}
//
//void LevelStatsPage_update(LevelStatsPage* self)
//{
//    Input* input = GameContext_getInput(self->m_context);
//    UIFocusManager_update(self->m_focusManager, &input->uiInput);
//
//    if (input->uiInput.cancelPressed || input->uiInput.validatePressed)
//    {
//        LevelUIManager* uiManager = self->m_uiManager;
//        LevelUIManager_closePages(uiManager);
//        LevelUIManager_openPage(uiManager, LEVEL_UI_PAGE_END);
//    }
//}
