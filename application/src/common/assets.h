/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"

// [TODO fighter]
// - Ajouter les sprites de l'ennemi Fighter dans cette énumération

typedef enum GameSpriteId
{
    SPRITE_UI_BASE,
    SPRITE_PARTICLE,
    SPRITE_BULLETS,
    SPRITE_PLAYER,
    SPRITE_PLAYER_SHIELD,
    SPRITE_ITEMS,
    //SPRITE_FIGHTER,
    //SPRITE_FRIGATE,
    //
    SPRITE_BACK_LAYER_0_A,
    SPRITE_BACK_LAYER_1,
    SPRITE_BACK_LAYER_2,
    //
    SPRITE_COUNT,
} GameSpriteId;

typedef enum GameFontId
{
    FONT_NORMAL,
    FONT_BIG,
    FONT_BOLD,
    FONT_BIG_BOLD,
    FONT_TITLE,
    FONT_HUD_LABEL,
    FONT_HUD_VALUE,
    //
    FONT_COUNT,
} GameFontId;

typedef enum GameAudioId
{
    AUDIO_MUSIC_0,
    AUDIO_MUSIC_1,
    //
    AUDIO_SFX_PLAYER_BASIC,
    AUDIO_SFX_PLAYER_SHIELD,
    AUDIO_SFX_ENEMY_EXPLOSION,
    AUDIO_SFX_ENEMY_BASIC,
    AUDIO_SFX_ITEM,
    //
    AUDIO_UI_SELECT,
    AUDIO_UI_VALIDATE,
    AUDIO_UI_CANCEL,
    AUDIO_UI_PAUSE,
    //
    AUDIO_COUNT,
} GameAudioId;

// [TODO fighter]
// - Ajouter les animations de l'ennemi Fighter (idle, firing, dying, shield) dans cette énumération

typedef enum GameAnimId
{
    // Player
    ANIM_PLAYER_SHIP,
    ANIM_PLAYER_ENGINE,
    ANIM_PLAYER_SHIELD,
    //// Fighter
    //ANIM_FIGHTER_IDLE,
    //ANIM_FIGHTER_FIRING,
    //ANIM_FIGHTER_DYING,
    //ANIM_FIGHTER_SHIELD,
    //// Frigate
    //ANIM_FRIGATE_IDLE,
    //ANIM_FRIGATE_FIRING,
    //ANIM_FRIGATE_DYING,
    //ANIM_FRIGATE_SHIELD,
    // Bullets
    ANIM_BULLET_BASIC_1,
    ANIM_BULLET_BASIC_2,
    ANIM_BULLET_ROCKET,
    // Items
    ANIM_ITEM_CURE,
    // Particles
    ANIM_PARTICLE_0,
    //
    ANIM_COUNT,
} GameAnimId;

void Game_addAssets(AssetManager* assets, SpriteAnimManager* animManager);
