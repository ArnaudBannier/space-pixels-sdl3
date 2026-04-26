/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/ui/title_level_menu.h"
#include "game/ui/title_ui_manager.h"
#include "game/core/game_context.h"
#include "common/ui_style.h"
#include "game/game_config.h"
#include "game/core/game_engine.h"

static void TitleLevelMenu_onClick(void* selectable)
{
    TitleLevelMenu* self = (TitleLevelMenu*)UISelectable_getUserData(selectable);
    assert(self && "self must not be NULL");

    SceneManager* sceneManager = GameEngine_getSceneManager(g_gameEngine);
    const char* objectName = UIObject_getObjectName(selectable);
    if (strcmp(objectName, "level-1-button") == 0)
    {
        g_gameConfig.level.levelId = LEVEL_1;
        SceneManager_changeScene(sceneManager, SCENE_TYPE_LEVEL);
    }
    else if (strcmp(objectName, "level-2-button") == 0)
    {
        g_gameConfig.level.levelId = LEVEL_2;
        TitleUIManager_closePages(self->m_uiManager);
        SceneManager_changeScene(sceneManager, SCENE_TYPE_LEVEL);
    }
    else if (strcmp(objectName, "back-button") == 0)
    {
        TitleUIManager_closePages(self->m_uiManager);
        TitleUIManager_openPage(self->m_uiManager, TITLE_UI_PAGE_MAIN_MENU);
    }
}

TitleLevelMenu* TitleLevelMenu_create(GameContext* context, TitleUIManager* manager)
{
    AssetManager* assets = GameContext_getAssetManager(context);
    UISystem* uiSystem = GameContext_getUISystem(context);
    UIObject* canvas = UISystem_getCanvas(uiSystem);
    UIRect rect = { 0 };
    TTF_Font* font = NULL;

    Input_clearUIInput(GameContext_getInput(context));

    TitleLevelMenu* self = (TitleLevelMenu*)calloc(1, sizeof(TitleLevelMenu));
    ASSERT_NEW(self);

    self->m_uiManager = manager;
    self->m_context = context;
    self->m_mainPanel = UIObject_create(uiSystem, "main-panel");
    self->m_focusManager = UIFocusManager_create(uiSystem);

    UIObject_setParent(self->m_mainPanel, canvas);
    rect.anchorMin = Vec2_set(0.5f, 0.0f);
    rect.anchorMax = Vec2_set(0.5f, 1.0f);
    rect.offsetMin = Vec2_set(-75.f, 0.0f);
    rect.offsetMax = Vec2_set(+75.f, 0.0f);
    UIObject_setRect(self->m_mainPanel, rect);

    UIGridLayout* layout = UIGridLayout_create(uiSystem, "main-layout", 4, 1);
    UIObject_setParent(layout, self->m_mainPanel);
    UIGridLayout_setRowSizes(layout, 25.0f);
    UIGridLayout_setRowSpacings(layout, 5.f);
    UIGridLayout_setRowSpacing(layout, 0, 20.f);
    UIGridLayout_setRowSpacing(layout, 2, 20.f);

    font = AssetManager_getFont(assets, FONT_TITLE);
    UILabel* label = UILabel_create(uiSystem, "title-label", font);
    UILabel_setTextString(label, "Level selection");
    UILabel_setAnchor(label, Vec2_anchor_center);
    UILabel_setColor(label, g_colors.white);

    UIGridLayout_addObject(layout, label, 0, 0, 1, 1);

    const char* buttonNames[2] = { "level-1-button", "level-2-button" };
    const char* buttonLabels[2] = {
        "Level 1",
        "Level 2",
    };
    font = AssetManager_getFont(assets, FONT_NORMAL);
    for (int i = 0; i < 2; i++)
    {
        UIButton* button = UIButton_create(uiSystem, buttonNames[i], font);
        UIButton_setLabelString(button, buttonLabels[i]);
        UIButton_setOnClickCallback(button, TitleLevelMenu_onClick);
        UISelectable_setUserData(button, self);
        UIStyle_setDefaultButton(button, assets);
        UIGridLayout_addObject(layout, button, i + 1, 0, 1, 1);
        UIFocusManager_addSelectable(self->m_focusManager, button);
        if (i == 0)
        {
            UIFocusManager_setFocused(self->m_focusManager, button);
        }
    }

    UIButton* button = UIButton_create(uiSystem, "back-button", font);
    UIButton_setLabelString(button, "Back");
    UIButton_setOnClickCallback(button, TitleLevelMenu_onClick);
    UISelectable_setUserData(button, self);
    UIStyle_setDefaultButton(button, assets);
    UIGridLayout_addObject(layout, button, 3, 0, 1, 1);
    UIFocusManager_addSelectable(self->m_focusManager, button);

    return self;
}

void TitleLevelMenu_destroy(TitleLevelMenu* self)
{
    if (!self) return;

    UIObject_destroy(self->m_mainPanel);
    UIFocusManager_destroy(self->m_focusManager);

    free(self);
}

void TitleLevelMenu_update(TitleLevelMenu* self)
{
    Input* input = GameContext_getInput(self->m_context);
    UIFocusManager_update(self->m_focusManager, &input->uiInput);
}
