/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/scene/level_scene.h"
#include "game/core/game_engine.h"

#include "game/level/fighter.h"

void LevelScene_updateLevel2(LevelScene* self)
{
    GameEngine* engine = Scene_getEngine(self);
    GameContext* context = GameEngine_getContext(engine);
    LevelInfo* level = &g_gameConfig.level;
    const ItemType noItem = ITEM_TYPE_NONE;

    switch (level->waveIdx)
    {
    case 0:
        // Fighter x4 - Frigate x1
        (void)Fighter_create(context, Vec2_set(5.f, -2.f), noItem);
        (void)Fighter_create(context, Vec2_set(6.f, -1.f), noItem);
        (void)Fighter_create(context, Vec2_set(6.f, +1.f), noItem);
        (void)Fighter_create(context, Vec2_set(5.f, +2.f), noItem);

        //(void)Frigate_create(context, Vec2_set(4.f, +0.f), noItem);
        break;

    default:
        LevelScene_onLevelCompleted(self);
        return;
    }

    level->waveIdx++;
}
