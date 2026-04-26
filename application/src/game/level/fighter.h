/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"
#include "game/core/game_object.h"
#include "game/level/enemy_data.h"

typedef struct Fighter
{
    GameObject m_base;
    EnemyData m_enemyData;
} Fighter;

Fighter* Fighter_create(GameContext* context, Vec2 position, ItemType itemType);
