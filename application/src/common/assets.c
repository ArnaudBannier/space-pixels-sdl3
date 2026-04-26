/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "common/assets.h"

typedef struct SheetLoadInfo
{
    int sheetId;
    const char* texPath;
    const char* descPath;
} SheetLoadInfo;

typedef struct FontLoadInfo
{
    GameFontId fontId;
    const char* path;
    float size;
} FontLoadInfo;

typedef struct AudioLoadInfo
{
    GameAudioId audioId;
    const char* path;
    float gain;
    bool predecode;
} AudioLoadInfo;

typedef struct AnimLoadInfo
{
    AnimId animId;
    int sheetId;
    const char* groupName;
    float fps;
    int loopCount;
} AnimLoadInfo;

void Game_addAssets(AssetManager* assets, SpriteAnimManager* animManager)
{
    // -------------------------------------------------------------------------
    // Sprite sheets

    // [TODO fighter]
    // - Ajouter les sprites de l'ennemi Fighter dans la liste des sprites à charger

    static const SheetLoadInfo sheetsToLoad[] = {
        { SPRITE_UI_BASE, "atlas/ui_base", "atlas/ui_base_desc" },
        { SPRITE_PARTICLE, "atlas/particles", "atlas/particles_desc" },
        { SPRITE_PLAYER, "atlas/player", "atlas/player_desc" },
        { SPRITE_PLAYER_SHIELD, "atlas/player_shield", "atlas/player_shield_desc" },
        { SPRITE_ITEMS, "atlas/items", "atlas/items_desc" },
        { SPRITE_BULLETS, "atlas/bullets", "atlas/bullets_desc" },
        //{ SPRITE_FIGHTER, "atlas/fighter", "atlas/fighter_desc" },
        //{ SPRITE_FRIGATE, "atlas/frigate", "atlas/frigate_desc" },
        { SPRITE_BACK_LAYER_0_A, "atlas/background/blue_nebula", "atlas/background/layer_desc" },
        { SPRITE_BACK_LAYER_1, "atlas/background/layer_01", "atlas/background/layer_desc" },
        { SPRITE_BACK_LAYER_2, "atlas/background/layer_02", "atlas/background/layer_desc" },
    };
    static const int sheetCount = sizeof(sheetsToLoad) / sizeof(SheetLoadInfo);

    for (int i = 0; i < sheetCount; i++)
    {
        AssetManager_addSpriteSheet(
            assets,
            sheetsToLoad[i].sheetId,
            sheetsToLoad[i].texPath,
            sheetsToLoad[i].descPath
        );
    }

    // -------------------------------------------------------------------------
    // Audios

    static const AudioLoadInfo audiosToLoad[] = {
        // Music
        { AUDIO_MUSIC_0, "audio/music/heatley_bros_luminare", 1.f, true },
        { AUDIO_MUSIC_1, "audio/music/heatley_bros_8_bit_power", 1.f, true },
        // SFX Player
        { AUDIO_SFX_PLAYER_BASIC, "audio/sound_fx/fire_01", 1.f, true },
        { AUDIO_SFX_PLAYER_SHIELD, "audio/sound_fx/shield", 1.f, true },
        // SFX Enemy
        { AUDIO_SFX_ENEMY_BASIC, "audio/sound_fx/fire_01", 0.5f, true },
        { AUDIO_SFX_ENEMY_EXPLOSION, "audio/sound_fx/explosion", 1.f, true },
        // Other SFX
        { AUDIO_SFX_ITEM, "audio/sound_fx/collect_item", 1.f, true },
        // UI
        { AUDIO_UI_CANCEL, "audio/sound_fx/ui_cancel", 1.f, true },
        { AUDIO_UI_PAUSE, "audio/sound_fx/ui_pause", 1.f, true },
        { AUDIO_UI_SELECT, "audio/sound_fx/ui_select", 1.f, true },
        { AUDIO_UI_VALIDATE, "audio/sound_fx/ui_validate", 1.f, true },
    };
    static const int audioCount = sizeof(audiosToLoad) / sizeof(AudioLoadInfo);

    for (int i = 0; i < audioCount; i++)
    {
        AssetManager_addAudio(
            assets,
            audiosToLoad[i].audioId,
            audiosToLoad[i].path,
            audiosToLoad[i].predecode
        );
        AssetManager_setAudioGain(
            assets,
            audiosToLoad[i].audioId,
            audiosToLoad[i].gain
        );
    }

    // -------------------------------------------------------------------------
    // Fonts

    static const FontLoadInfo fontsToLoad[] = {
        { FONT_NORMAL, "font/noto_sans_regular", 32 },
        { FONT_BOLD, "font/noto_sans_bold", 32 },
        { FONT_BIG, "font/noto_sans_regular", 64 },
        { FONT_BIG_BOLD, "font/noto_sans_bold", 64 },
        { FONT_TITLE, "font/futile_pro", 96 },
        { FONT_HUD_LABEL, "font/m5x7", 32 },
        { FONT_HUD_VALUE, "font/futile_pro", 48 },
    };
    static const int fontCount = sizeof(fontsToLoad) / sizeof(FontLoadInfo);

    for (int i = 0; i < fontCount; i++)
    {
        AssetManager_addFont(
            assets,
            fontsToLoad[i].fontId,
            fontsToLoad[i].path,
            fontsToLoad[i].size
        );
    }

    // -------------------------------------------------------------------------
    // Animations

    // [TODO fighter]
    // - Ajouter les animations de l'ennemi Fighter dans la liste des animations à charger

    static const AnimLoadInfo animsToLoad[] = {
        { ANIM_PARTICLE_0, SPRITE_PARTICLE, "particle-0", 24.f, -1 },
        // Player
        { ANIM_PLAYER_SHIP, SPRITE_PLAYER, "ship", 1.f, -1 },
        { ANIM_PLAYER_ENGINE, SPRITE_PLAYER, "engine", 15.f, -1 },
        { ANIM_PLAYER_SHIELD, SPRITE_PLAYER_SHIELD, "shield", 30.f, -1 },
        // Bullets
        { ANIM_BULLET_BASIC_1, SPRITE_BULLETS, "basic-1", 10.f, -1 },
        { ANIM_BULLET_BASIC_2, SPRITE_BULLETS, "basic-2", 10.f, -1 },
        { ANIM_BULLET_ROCKET, SPRITE_BULLETS, "rocket", 10.f, -1 },
        // Items
        { ANIM_ITEM_CURE, SPRITE_ITEMS, "cure", 16.f, -1 },
        //// Fighter
        //{ ANIM_FIGHTER_IDLE, SPRITE_FIGHTER, "idle", 6.f, -1 },
        //{ ANIM_FIGHTER_FIRING, SPRITE_FIGHTER, "firing", 6.f, -1 },
        //{ ANIM_FIGHTER_DYING, SPRITE_FIGHTER, "dying", 12.f, 1 },
        //{ ANIM_FIGHTER_SHIELD, SPRITE_FIGHTER, "shield", 18.f, -1 },
        //// Frigate
        //{ ANIM_FRIGATE_IDLE, SPRITE_FRIGATE, "idle", 6.f, -1 },
        //{ ANIM_FRIGATE_FIRING, SPRITE_FRIGATE, "firing", 6.f, 3 },
        //{ ANIM_FRIGATE_DYING, SPRITE_FRIGATE, "dying", 12.f, 1 },
        //{ ANIM_FRIGATE_SHIELD, SPRITE_FRIGATE, "shield", 18.f, -1 },
    };
    static const int animCount = sizeof(animsToLoad) / sizeof(AnimLoadInfo);

    for (int i = 0; i < animCount; i++)
    {
        SpriteSheet* spriteSheet = AssetManager_getSpriteSheet(
            assets,
            animsToLoad[i].sheetId
        );
        ASSERT_NEW(spriteSheet);
        SpriteGroup* spriteGroup = SpriteSheet_getGroupByName(
            spriteSheet,
            animsToLoad[i].groupName
        );
        ASSERT_NEW(spriteGroup);

        SpriteAnimManager_addSpriteAnim(
            animManager,
            animsToLoad[i].animId,
            spriteGroup
        );
        SpriteAnim* anim = SpriteAnimManager_getSpriteAnim(
            animManager,
            animsToLoad[i].animId
        );
        ASSERT_NEW(anim);
        SpriteAnim_setFPS(anim, animsToLoad[i].fps);
        SpriteAnim_setCycleCount(anim, animsToLoad[i].loopCount);
    }
}
