/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"
#include "game/core/game_object.h"
#include "game/level/item.h"

typedef enum EnemyState
{
    ENEMY_STATE_FIRING,
    ENEMY_STATE_DYING,
} EnemyState;

typedef struct EnemyData
{
    EnemyState state;

    Vec2 position;
    float hp;
    float points;
    bool hasShield;

    AnimId idleAnimId;
    AnimId firingAnimId;
    AnimId dyingAnimId;
    AnimId shieldAnimId;

    ItemType itemType;

    SpriteAnimState m_mainAnim;
    SpriteAnimState m_shieldAnim;
    RenderDim m_renderDim;
} EnemyData;

void Enemy_init(void* selfPtr, EnemyData* data);
void Enemy_onUpdate(void* selfPtr, EnemyData* data, float dt);
void Enemy_onFixedUpdate(void* selfPtr, EnemyData* data, float timeStep);
void Enemy_onTakeDamage(void* selfPtr, EnemyData* data, float damage, int playerId);
void Enemy_onRender(void* selfPtr, EnemyData* data);
void Enemy_onDrawGizmos(void* selfPtr, EnemyData* data, SDL_Color* color);
void Enemy_onDestroy(void* selfPtr, EnemyData* data);
