/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/scene/level_scene.h"
#include "game/core/game_engine.h"

#include "game/level/player.h"
#include "game/level/item.h"
#include "game/level/background_layer.h"

static void LevelScene_onStart(void* selfPtr);
static void LevelScene_onQuit(void* selfPtr);
static void LevelScene_onUpdate(void* selfPtr, float dt);
static void LevelScene_onFixedUpdate(void* selfPtr, float timeStep);
static void LevelScene_onDestroy(void* selfPtr);

static void LevelScene_updateGame(LevelScene* self);

LevelScene* LevelScene_create(GameEngine* engine)
{
    LevelScene* self = (LevelScene*)calloc(1, sizeof(LevelScene));
    ASSERT_NEW(self);

    Scene* selfScene = (Scene*)self;
    Scene_init(selfScene, engine);
    selfScene->m_onStart = LevelScene_onStart;
    selfScene->m_onQuit = LevelScene_onQuit;
    selfScene->m_onUpdate = LevelScene_onUpdate;
    selfScene->m_onFixedUpdate = LevelScene_onFixedUpdate;
    selfScene->m_onDestroy = LevelScene_onDestroy;

    GameContext* context = GameEngine_getContext(engine);
    self->m_uiManager = LevelUIManager_create(context);

    return self;
}

static void LevelScene_onStart(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    LevelScene* self = (LevelScene*)selfPtr;
    GameContext* context = Scene_getContext(self);

    LevelUIManager_openPage(self->m_uiManager, LEVEL_UI_PAGE_HUD);

    const int playerCount = g_gameConfig.playerCount;
    assert(playerCount > 0 && playerCount <= MAX_PLAYER_COUNT);

    LevelInfo* level = &g_gameConfig.level;
    level->waveIdx = 0;
    level->enemyCount = 0;
    level->activePlayerCount = 0;
    level->state = GAME_STATE_PLAYING;

    for (int i = 0; i < playerCount; i++)
    {
        Vec2 position = Vec2_set(-4.f, (playerCount - 1) * -0.75f + i * 1.5f);
        Player* player = Player_create(context, position, i);
    }

    AssetManager* assets = GameContext_getAssetManager(context);
    AudioSystem* audioSystem = GameContext_getAudioSystem(context);
    AudioSystem_stopGroup(audioSystem, GAME_AUDIO_GROUP_MUSIC, 1000);

    MIX_Audio* music = AssetManager_getAudio(assets, AUDIO_MUSIC_0);
    AudioPlayOptions options = { 0 };
    AudioPlayOptions_init(&options);
    options.startTimeMS = 1000;
    options.loopCount = -1;
    AudioSystem_play(audioSystem, GAME_AUDIO_GROUP_MUSIC, music, &options);

    GameSpriteId backgroundId = SPRITE_BACK_LAYER_0_A;
    GameSpriteId layerIds[] = {
        backgroundId,
        SPRITE_BACK_LAYER_1,
        SPRITE_BACK_LAYER_2
    };
    for (int i = 0; i < 3; i++)
    {
        SpriteSheet* spriteSheet = AssetManager_getSpriteSheet(assets, layerIds[i]);
        ASSERT_NEW(spriteSheet);
        SpriteGroup* spriteGroup = SpriteSheet_getGroupByIndex(spriteSheet, 0);
        ASSERT_NEW(spriteGroup);

        BackgroundLayer* layer = BackgroundLayer_create(
            context,
            Vec2_set(1024 / PIX_PER_UNIT, 1024 / PIX_PER_UNIT),
            Vec2_set(-0.25f * i - 0.1f, 0.f),
            spriteGroup, 0, i
        );
    }

    LevelScene_updateGame(self);
}

static void LevelScene_onQuit(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    LevelScene* self = (LevelScene*)selfPtr;
    GameContext* context = Scene_getContext(self);
    AudioSystem* audioSystem = GameContext_getAudioSystem(context);

    ObjectManager* objectManager = GameContext_getObjectManager(context);
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    ObjectManager_clear(objectManager);
    PhysicsEngine_clear(physicsEngine);

    LevelUIManager_closePages(self->m_uiManager);
    LevelUIManager_update(self->m_uiManager);

    Timer_setTimeScale(g_engine.time, 1.f);
    AudioSystem_setGain(audioSystem, GAME_AUDIO_GROUP_MUSIC, g_gameConfig.musicGain);
}

static void LevelScene_updateGame(LevelScene* self)
{
    LevelInfo* level = &g_gameConfig.level;

    if ((level->state == GAME_STATE_COMPLETED) ||
        (level->state == GAME_STATE_FAILED))
        return;

    if (level->state == GAME_STATE_PLAYING)
    {
        if (level->activePlayerCount <= 0)
        {
            LevelScene_onLevelFailed(self);
            return;
        }

        // Wait for all enemies to be destroyed before spawning the next wave.
        if (level->enemyCount > 0) return;

        switch (level->levelId)
        {
        case LEVEL_1: LevelScene_updateLevel1(self); break;

        // [TODO level]
        // - Mettez à jour la fonction LevelScene_updateGame pour appeler une nouvelle fonction LevelScene_updateLevel2.
        
        //case LEVEL_2: LevelScene_updateLevel2(self); break;

        default:
            assert(false && "Invalid levelId");
            break;
        }
    }
}

static void LevelScene_onUpdate(void* selfPtr, float dt)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    LevelScene* self = (LevelScene*)selfPtr;
    GameContext* context = Scene_getContext(self);
    Input* input = GameContext_getInput(context);

    if (g_gameConfig.level.state == GAME_STATE_PLAYING)
    {
        if (input->app.pausePressed || g_gameConfig.level.shouldPause)
        {
            g_gameConfig.level.shouldPause = false;
            LevelScene_onLevelPaused(self);
        }
    }
    else if (g_gameConfig.level.state == GAME_STATE_PAUSED)
    {
        if (input->app.pausePressed || g_gameConfig.level.shouldResume)
        {
            LevelScene_onLevelResumed(self);
        }
    }
    g_gameConfig.level.shouldResume = false;
    g_gameConfig.level.shouldPause = false;

    LevelScene_updateGame(self);
    LevelUIManager_update(self->m_uiManager);
}

static void LevelScene_onFixedUpdate(void* selfPtr, float timeStep)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    LevelScene* self = (LevelScene*)selfPtr;
}

static void LevelScene_onDestroy(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    LevelScene* self = (LevelScene*)selfPtr;
}

void LevelScene_onLevelCompleted(LevelScene* self)
{
    assert(self && "self must not be NULL");
    g_gameConfig.level.state = GAME_STATE_COMPLETED;

    // [TODO level]
    // - Ouvrez la page de fin de niveau (LEVEL_UI_PAGE_END)
    // - Mettez le temps à l'échelle 0 pour geler le jeu
    
    //LevelUIManager_openPage(self->m_uiManager, LEVEL_UI_PAGE_END);
    //Timer_setTimeScale(g_engine.time, 0.f);
}

void LevelScene_onLevelFailed(LevelScene* self)
{
    assert(self && "self must not be NULL");
    g_gameConfig.level.state = GAME_STATE_FAILED;

    // [TODO level]
    // - Ouvrez la page de fin de niveau (LEVEL_UI_PAGE_END)
    // - Mettez le temps à l'échelle 0 pour geler le jeu

    //LevelUIManager_openPage(self->m_uiManager, LEVEL_UI_PAGE_END);
    //Timer_setTimeScale(g_engine.time, 0.f);
}

void LevelScene_onLevelPaused(LevelScene* self)
{
    assert(self && "self must not be NULL");
    GameContext* context = Scene_getContext(self);
    AudioSystem* audioSystem = GameContext_getAudioSystem(context);

    g_gameConfig.level.state = GAME_STATE_PAUSED;

    Timer_setTimeScale(g_engine.time, 0.f);
    AudioSystem_setGain(audioSystem, GAME_AUDIO_GROUP_MUSIC, 0.5f * g_gameConfig.musicGain);

    LevelUIManager_openPage(self->m_uiManager, LEVEL_UI_PAGE_PAUSE);
}

void LevelScene_onLevelResumed(LevelScene* self)
{
    assert(self && "self must not be NULL");
    GameContext* context = Scene_getContext(self);
    AudioSystem* audioSystem = GameContext_getAudioSystem(context);

    g_gameConfig.level.state = GAME_STATE_PLAYING;

    Timer_setTimeScale(g_engine.time, 1.f);
    AudioSystem_setGain(audioSystem, GAME_AUDIO_GROUP_MUSIC, g_gameConfig.musicGain);

    LevelUIManager_closePage(self->m_uiManager, LEVEL_UI_PAGE_PAUSE);
}
