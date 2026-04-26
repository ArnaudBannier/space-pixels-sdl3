/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/ui/level_pause_page.h"
#include "game/ui/title_ui_manager.h"
#include "game/core/game_context.h"
#include "common/ui_style.h"
#include "game/game_config.h"
#include "game/core/game_engine.h"

static void LevelPausePage_onClick(void* selectable)
{
    LevelPausePage* self = (LevelPausePage*)UISelectable_getUserData(selectable);
    assert(self && "self must not be NULL");

    const char* objectName = UIObject_getObjectName(selectable);
    if (strcmp(objectName, "resume-button") == 0)
    {
        g_gameConfig.level.shouldResume = true;
    }
    else if (strcmp(objectName, "main-menu-button") == 0)
    {
        SceneManager* sceneManager = GameEngine_getSceneManager(g_gameEngine);
        SceneManager_changeScene(sceneManager, SCENE_TYPE_TITLE);
    }
}

LevelPausePage* LevelPausePage_create(GameContext* context, LevelUIManager* manager)
{
    AssetManager* assets = GameContext_getAssetManager(context);
    UISystem* uiSystem = GameContext_getUISystem(context);
    UIObject* canvas = UISystem_getCanvas(uiSystem);
    UIRect rect = { 0 };
    RenderSortingLayer sortingLayer = { 0 };
    TTF_Font* font = NULL;
    UILabel* label = NULL;

    Input_clearUIInput(GameContext_getInput(context));

    LevelPausePage* self = (LevelPausePage*)calloc(1, sizeof(LevelPausePage));
    ASSERT_NEW(self);

    self->m_uiManager = manager;
    self->m_context = context;
    self->m_mainPanel = UIObject_create(uiSystem, "main-panel");
    self->m_focusManager = UIFocusManager_create(uiSystem);

    UIObject_setParent(self->m_mainPanel, canvas);
    rect.anchorMin = Vec2_set(0.0f, 0.3f);
    rect.anchorMax = Vec2_set(1.0f, 0.7f);
    rect.offsetMin = Vec2_set(0.0f, 0.0f);
    rect.offsetMax = Vec2_set(0.0f, 0.0f);
    UIObject_setRect(self->m_mainPanel, rect);

    UIGridLayout* layout = UIGridLayout_create(uiSystem, "main-layout", 3, 1);
    UIObject_setParent(layout, self->m_mainPanel);
    UIGridLayout_setRowSizes(layout, 25.0f);
    UIGridLayout_setRowSpacings(layout, 5.f);
    UIGridLayout_setRowSpacing(layout, 0, 20.f);
    UIGridLayout_setColumnSizes(layout, 150.f);

    font = AssetManager_getFont(assets, FONT_TITLE);
    label = UILabel_create(uiSystem, "title-label", font);
    UILabel_setTextString(label, "Pause");
    UILabel_setAnchor(label, Vec2_anchor_center);
    UILabel_setColor(label, g_colors.white);

    UIGridLayout_addObject(layout, label, 0, 0, 1, 1);

    const char* buttonNames[2] = { "resume-button", "main-menu-button" };
    const char* buttonLabels[2] = { "Resume", "Main menu" };
    font = AssetManager_getFont(assets, FONT_NORMAL);
    for (int i = 0; i < 2; i++)
    {
        UIButton* button = UIButton_create(uiSystem, buttonNames[i], font);
        UIButton_setLabelString(button, buttonLabels[i]);
        UIButton_setOnClickCallback(button, LevelPausePage_onClick);
        UISelectable_setUserData(button, self);
        UIStyle_setDefaultButton(button, assets);
        UIGridLayout_addObject(layout, button, i + 1, 0, 1, 1);
        UIFocusManager_addSelectable(self->m_focusManager, button);
        if (i == 0)
        {
            UIFocusManager_setFocused(self->m_focusManager, button);
        }
    }

    UIFillRect* background = UIFillRect_create(uiSystem, "background", g_colors.black);
    UIObject_setParent(background, self->m_mainPanel);
    sortingLayer.layer = GAME_LAYER_UI;
    sortingLayer.orderInLayer = -1;
    UIObject_setRenderLayer(background, sortingLayer);
    UIFillRect_setOpacity(background, 0.5f);

    return self;
}

void LevelPausePage_destroy(LevelPausePage* self)
{
    if (!self) return;

    UIFocusManager_destroy(self->m_focusManager);
    UIObject_destroy(self->m_mainPanel);

    free(self);
}

void LevelPausePage_update(LevelPausePage* self)
{
    Input* input = GameContext_getInput(self->m_context);
    UIFocusManager_update(self->m_focusManager, &input->uiInput);
}
