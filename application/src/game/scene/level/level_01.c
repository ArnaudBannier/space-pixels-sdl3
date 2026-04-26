/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/scene/level_scene.h"
#include "game/core/game_engine.h"

#include "game/level/fighter.h"
#include "game/level/item.h"

void LevelScene_updateLevel1(LevelScene* self)
{
    GameEngine* engine = Scene_getEngine(self);
    GameContext* context = GameEngine_getContext(engine);
    LevelInfo* level = &g_gameConfig.level;

    switch (level->waveIdx)
    {
    case 0:
        (void)Fighter_create(context, Vec2_set(6.0f, 0.f), ITEM_TYPE_CURE);
        break;

    case 1:
        (void)Fighter_create(context, Vec2_set(4.0f, 0.f), ITEM_TYPE_CURE);
        break;

        // [TODO level]
        // - Ajoutez une nouvelle vague d'ennemis à la position (5, -2) et (5, 2) qui lâchent des items de soin à leur mort.

        //case 2:
        //    (void)Fighter_create(context, Vec2_set(5.0f, -2.f), ITEM_TYPE_CURE);
        //    (void)Fighter_create(context, Vec2_set(5.0f, +2.f), ITEM_TYPE_CURE);
        //    break;

    default:
        LevelScene_onLevelCompleted(self);
        return;
    }

    level->waveIdx++;
}
