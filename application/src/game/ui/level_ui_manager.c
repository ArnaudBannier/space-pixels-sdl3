/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/ui/level_ui_manager.h"
#include "game/ui/level_hud.h"
#include "game/ui/level_pause_page.h"
#include "game/ui/level_end_page.h"
#include "game/core/game_context.h"
#include "game/game_config.h"

LevelUIManager* LevelUIManager_create(GameContext* context)
{
    LevelUIManager* self = (LevelUIManager*)calloc(1, sizeof(LevelUIManager));
    ASSERT_NEW(self);

    self->m_context = context;
    self->m_hud = NULL;
    self->m_pausePage = NULL;
    self->m_endPage = NULL;

    // [TODO stats page]
    //self->m_statsPage = NULL;

    return self;
}

void LevelUIManager_destroy(LevelUIManager* self)
{
    if (!self) return;

    LevelHUD_destroy(self->m_hud);
    LevelPausePage_destroy(self->m_pausePage);
    LevelEndPage_destroy(self->m_endPage);
    // [TODO stats page]
    //LevelStatsPage_destroy(self->m_statsPage);
    free(self);
}

static void LevelUIManager_applyPageChange(LevelUIManager* self)
{
    // HUD
    if (self->m_activePages[LEVEL_UI_PAGE_HUD])
    {
        if (!self->m_hud)
        {
            self->m_hud = LevelHUD_create(self->m_context, self);
        }
    }
    else
    {
        LevelHUD_destroy(self->m_hud);
        self->m_hud = NULL;
    }

    // Pause page
    if (self->m_activePages[LEVEL_UI_PAGE_PAUSE])
    {
        if (!self->m_pausePage)
        {
            self->m_pausePage = LevelPausePage_create(self->m_context, self);
        }
    }
    else
    {
        LevelPausePage_destroy(self->m_pausePage);
        self->m_pausePage = NULL;
    }

    // End page
    if (self->m_activePages[LEVEL_UI_PAGE_END])
    {
        if (!self->m_endPage)
        {
            self->m_endPage = LevelEndPage_create(self->m_context, self);
        }
    }
    else
    {
        LevelEndPage_destroy(self->m_endPage);
        self->m_endPage = NULL;
    }

    // [TODO stats page]
    //// Stats page
    //if (self->m_activePages[LEVEL_UI_PAGE_STATS])
    //{
    //    if (!self->m_statsPage)
    //    {
    //        self->m_statsPage = LevelStatsPage_create(self->m_context, self);
    //    }
    //}
    //else
    //{
    //    LevelStatsPage_destroy(self->m_statsPage);
    //    self->m_statsPage = NULL;
    //}
}

void LevelUIManager_update(LevelUIManager* self)
{
    assert(self && "self must not be NULL");

    LevelUIManager_applyPageChange(self);

    if (self->m_hud)
    {
        LevelHUD_update(self->m_hud);
    }
    if (self->m_pausePage)
    {
        LevelPausePage_update(self->m_pausePage);
    }
    if (self->m_endPage)
    {
        LevelEndPage_update(self->m_endPage);
    }
    // [TODO stats page]
    //if (self->m_statsPage)
    //{
    //    LevelStatsPage_update(self->m_statsPage);
    //}
}
