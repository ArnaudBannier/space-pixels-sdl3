/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/ui/level_end_page.h"
#include "game/ui/title_ui_manager.h"
#include "game/core/game_context.h"
#include "common/ui_style.h"
#include "game/game_config.h"
#include "game/core/game_engine.h"

// [TODO level end page]
//static void LevelEndPage_onClick(void* selectable)
//{
//    LevelEndPage* self = (LevelEndPage*)UISelectable_getUserData(selectable);
//    assert(self && "self must not be NULL");
//
//    const char* objectName = UIObject_getObjectName(selectable);
//    if (strcmp(objectName, "main-menu-button") == 0)
//    {
//        SceneManager* sceneManager = GameEngine_getSceneManager(g_gameEngine);
//        SceneManager_changeScene(sceneManager, SCENE_TYPE_TITLE);
//    }
//    else if (strcmp(objectName, "stats-button") == 0)
//    {
//        // [TODO stats page]
//        //LevelUIManager_closePages(self->m_uiManager);
//        //LevelUIManager_openPage(self->m_uiManager, LEVEL_UI_PAGE_STATS);
//    }
//}

LevelEndPage* LevelEndPage_create(GameContext* context, LevelUIManager* manager)
{
    AssetManager* assets = GameContext_getAssetManager(context);
    UISystem* uiSystem = GameContext_getUISystem(context);
    UIObject* canvas = UISystem_getCanvas(uiSystem);
    UIRect rect = { 0 };
    RenderSortingLayer sortingLayer = { 0 };
    TTF_Font* font = NULL;
    UILabel* label = NULL;

    Input_clearUIInput(GameContext_getInput(context));

    LevelEndPage* self = (LevelEndPage*)calloc(1, sizeof(LevelEndPage));
    ASSERT_NEW(self);

    self->m_uiManager = manager;
    self->m_context = context;
    self->m_mainPanel = UIObject_create(uiSystem, "main-panel");
    self->m_focusManager = UIFocusManager_create(uiSystem);

    UIObject_setParent(self->m_mainPanel, canvas);

    // Background
    UIFillRect* background = UIFillRect_create(uiSystem, "background", g_colors.black);
    UIObject_setParent(background, self->m_mainPanel);
    sortingLayer.layer = GAME_LAYER_UI;
    sortingLayer.orderInLayer = -1;
    UIObject_setRenderLayer(background, sortingLayer);
    UIFillRect_setOpacity(background, 0.5f);

    // [TODO level end page]
    //// Layout
    //UIGridLayout* layout = UIGridLayout_create(uiSystem, "main-layout", 3, 1);
    //UIObject_setParent(layout, self->m_mainPanel);
    //UIGridLayout_setRowSizes(layout, 25.0f);
    //UIGridLayout_setRowSpacings(layout, 5.f);
    //UIGridLayout_setRowSpacing(layout, 0, 20.f);
    //UIGridLayout_setColumnSizes(layout, 150.0f);

    // Title
    font = AssetManager_getFont(assets, FONT_TITLE);
    label = UILabel_create(uiSystem, "title-label", font);
    if (g_gameConfig.level.state == GAME_STATE_COMPLETED)
    {
        UILabel_setTextString(label, "Victory!");
    }
    else
    {
        UILabel_setTextString(label, "Defeat!");
    }
    UILabel_setAnchor(label, Vec2_anchor_center);
    UILabel_setColor(label, g_colors.white);

    // [TODO level end page]
    // - Supprimer pour utiliser le layout
    UIObject_setParent(label, self->m_mainPanel);

    //UIGridLayout_addObject(layout, label, 0, 0, 1, 1);

    // [TODO level end page]
    //const char* buttonNames[2] = { "main-menu-button", "stats-button" };
    //const char* buttonLabels[2] = { "Main Menu", "Stats" };
    //font = AssetManager_getFont(assets, FONT_NORMAL);
    //for (int i = 0; i < 2; i++)
    //{
    //    UIButton* button = UIButton_create(uiSystem, buttonNames[i], font);
    //    UIButton_setLabelString(button, buttonLabels[i]);
    //    UIButton_setOnClickCallback(button, LevelEndPage_onClick);
    //    UISelectable_setUserData(button, self);
    //    UIStyle_setDefaultButton(button, assets);
    //    UIGridLayout_addObject(layout, button, i + 1, 0, 1, 1);
    //    UIFocusManager_addSelectable(self->m_focusManager, button);
    //    if (i == 0)
    //    {
    //        UIFocusManager_setFocused(self->m_focusManager, button);
    //    }
    //}

    return self;
}

void LevelEndPage_destroy(LevelEndPage* self)
{
    if (!self) return;

    UIFocusManager_destroy(self->m_focusManager);
    UIObject_destroy(self->m_mainPanel);

    free(self);
}

void LevelEndPage_update(LevelEndPage* self)
{
    Input* input = GameContext_getInput(self->m_context);
    UIFocusManager_update(self->m_focusManager, &input->uiInput);

    // [TODO level end page]
    // - Supprimer pour utiliser les boutons
    if (input->uiInput.validatePressed || input->app.pausePressed)
    {
        SceneManager* sceneManager = GameEngine_getSceneManager(g_gameEngine);
        SceneManager_changeScene(sceneManager, SCENE_TYPE_TITLE);
    }
}
