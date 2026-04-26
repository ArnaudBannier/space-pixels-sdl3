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

    font = AssetManager_getFont(assets, FONT_TITLE);
    label = UILabel_create(uiSystem, "title-label", font);
    UIObject_setParent(label, self->m_mainPanel);
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

    UIFillRect* background = UIFillRect_create(uiSystem, "background", g_colors.black);
    UIObject_setParent(background, self->m_mainPanel);
    sortingLayer.layer = GAME_LAYER_UI;
    sortingLayer.orderInLayer = -1;
    UIObject_setRenderLayer(background, sortingLayer);
    UIFillRect_setOpacity(background, 0.5f);

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

    if (input->uiInput.validatePressed || input->app.pausePressed)
    {
        SceneManager* sceneManager = GameEngine_getSceneManager(g_gameEngine);
        SceneManager_changeScene(sceneManager, SCENE_TYPE_TITLE);
    }
}
