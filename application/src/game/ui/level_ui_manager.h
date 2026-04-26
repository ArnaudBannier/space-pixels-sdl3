/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"

typedef struct GameContext GameContext;
typedef struct LevelHUD LevelHUD;
typedef struct LevelPausePage LevelPausePage;
typedef struct LevelEndPage LevelEndPage;

typedef enum LevelUIPageId
{
    LEVEL_UI_PAGE_HUD = 0,
    LEVEL_UI_PAGE_PAUSE,
    LEVEL_UI_PAGE_END,
    //
    LEVEL_UI_PAGE_COUNT
} LevelUIPageId;

typedef struct LevelUIManager
{
    GameContext* m_context;
    bool m_activePages[LEVEL_UI_PAGE_COUNT];

    LevelHUD* m_hud;
    LevelPausePage* m_pausePage;
    LevelEndPage* m_endPage;
} LevelUIManager;

LevelUIManager* LevelUIManager_create(GameContext* context);
void LevelUIManager_destroy(LevelUIManager* self);
void LevelUIManager_update(LevelUIManager* self);

INLINE void LevelUIManager_openPage(LevelUIManager* self, LevelUIPageId pageId)
{
    assert(self && "self must not be NULL");
    if (pageId < 0 || pageId >= LEVEL_UI_PAGE_COUNT)
    {
        assert(false && "Invalid page");
        return;
    }
    self->m_activePages[pageId] = true;
}

INLINE void LevelUIManager_closePage(LevelUIManager* self, LevelUIPageId pageId)
{
    assert(self && "self must not be NULL");
    if (pageId < 0 || pageId >= LEVEL_UI_PAGE_COUNT)
    {
        assert(false && "Invalid page");
        return;
    }
    self->m_activePages[pageId] = false;
}

INLINE void LevelUIManager_closePages(LevelUIManager* self)
{
    assert(self && "self must not be NULL");
    for (int i = 0; i < LEVEL_UI_PAGE_COUNT; ++i)
    {
        self->m_activePages[i] = false;
    }
}
