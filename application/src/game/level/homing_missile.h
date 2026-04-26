/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"
#include "game/core/game_object.h"

typedef enum HomingMissileType
{
    HOMING_MISSILE_TYPE_PLAYER_ROCKET,
} HomingMissileType;

typedef struct HomingMissile
{
    GameObject m_base;
    HomingMissileType m_type;
    float m_damage;
    int m_playerId;

    SpriteAnimState m_mainAnim;
    RenderDim m_renderDim;

    float m_speed;
    float m_homingTime;
    float m_searchRadius;
    float m_angle;
    float m_angularVelocity;

    uint32_t m_targetMask;
    BodyId m_targetBodyId;
} HomingMissile;

HomingMissile* HomingMissile_create(
    GameContext* context, HomingMissileType type,
    Vec2 position, float speed, Vec2 direction, int playerId);

