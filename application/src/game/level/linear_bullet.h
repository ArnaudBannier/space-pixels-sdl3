/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"
#include "game/core/game_object.h"

typedef enum LinearBulletType
{
    LINEAR_BULLET_TYPE_PLAYER_DEFAULT,
    LINEAR_BULLET_TYPE_FIGHTER,
    LINEAR_BULLET_TYPE_FRIGATE,
} LinearBulletType;

typedef struct LinearBullet
{
    GameObject m_base;
    LinearBulletType m_type;
    float m_damage;
    int m_playerId;

    SpriteAnimState m_mainAnim;
    RenderDim m_renderDim;
} LinearBullet;

LinearBullet* LinearBullet_create(
    GameContext* context, LinearBulletType type,
    Vec2 position, float speed, Vec2 direction, int playerId);

