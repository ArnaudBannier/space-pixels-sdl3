/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"
#include "game/input.h"
#include "game/core/physics_engine.h"
#include "game/core/game_object.h"

typedef struct Scene Scene;
typedef struct GameContext GameContext;

typedef enum PlayerState
{
    PLAYER_STATE_FLYING,
    PLAYER_STATE_DYING,
    PLAYER_STATE_DEAD,
} PlayerState;

typedef enum BonusFlag
{
    BONUS_AUTO_CANONS = 1 << 0,
    BONUS_ROCKETS = 1 << 1,
    BONUS_BIG_GUN = 1 << 2,
} BonusFlag;

typedef struct Player
{
    GameObject m_base;

    Vec2 m_direction;
    float m_speed;

    SpriteAnimState m_mainAnim;
    SpriteAnimState m_engineAnim;
    SpriteAnimState m_shieldAnim;

    RenderDim m_renderDim;

    float m_maxHp;

    /// @brief Etat du joueur.
    /// Les valeurs possibles sont données dans PlayerState.
    int m_state;

    /// @brief Identifiant du joueur (entre 0 et playerCount - 1).
    int m_playerId;

    // [TODO Player shoot] V2
    // - Ajouter un accumulateur pour les tirs du joueur afin de limiter leur cadence de tir.

    /// @brief Accumulateur pour les tirs par défaut.
    //float m_accuBullet;

    /// @brief Accumulateur pour le bouclier.
    float m_accuShield;

    int m_bonusFlags;
} Player;

Player* Player_create(GameContext* context, Vec2 position, int playerId);
void Player_addBonus(void* selfPtr, int flags);
void Player_addPoints(void* selfPtr, float points);
void Player_heal(void* selfPtr, float hp);
