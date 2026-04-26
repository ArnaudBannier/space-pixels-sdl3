/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"
#include "game/core/scene_manager.h"
#include "game/core/game_context.h"
#include "game/ui/level_ui_manager.h"

typedef struct LevelScene
{
    Scene m_base;
    LevelUIManager* m_uiManager;
} LevelScene;

LevelScene* LevelScene_create(GameEngine* engine);

void LevelScene_onLevelCompleted(LevelScene* self);
void LevelScene_onLevelFailed(LevelScene* self);
void LevelScene_onLevelPaused(LevelScene* self);
void LevelScene_onLevelResumed(LevelScene* self);

void LevelScene_updateLevel1(LevelScene* self);
void LevelScene_updateLevel2(LevelScene* self);
