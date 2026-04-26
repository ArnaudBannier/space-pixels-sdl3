/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/scene/title_scene.h"
#include "game/core/game_engine.h"
#include "game/level/background_layer.h"

static void TitleScene_onStart(void* selfPtr);
static void TitleScene_onQuit(void* selfPtr);
static void TitleScene_onUpdate(void* selfPtr, float dt);
static void TitleScene_onFixedUpdate(void* selfPtr, float timeStep);
static void TitleScene_onDestroy(void* selfPtr);

TitleScene* TitleScene_create(GameEngine* engine)
{
    TitleScene* self = (TitleScene*)calloc(1, sizeof(TitleScene));
    ASSERT_NEW(self);

    Scene* selfScene = (Scene*)self;
    Scene_init(selfScene, engine);
    selfScene->m_onStart = TitleScene_onStart;
    selfScene->m_onQuit = TitleScene_onQuit;
    selfScene->m_onUpdate = TitleScene_onUpdate;
    selfScene->m_onFixedUpdate = TitleScene_onFixedUpdate;
    selfScene->m_onDestroy = TitleScene_onDestroy;

    GameContext* context = GameEngine_getContext(engine);
    self->m_uiManager = TitleUIManager_create(context);

    return self;
}

static void TitleScene_onStart(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    TitleScene* self = (TitleScene*)selfPtr;
    GameContext* context = Scene_getContext(self);

    TitleUIManager_openPage(self->m_uiManager, TITLE_UI_PAGE_MAIN_MENU);

    ObjectManager* objectManager = GameContext_getObjectManager(context);
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    ObjectManager_clear(objectManager);
    PhysicsEngine_clear(physicsEngine);

    AssetManager* assets = GameContext_getAssetManager(context);
    AudioSystem* audioSystem = GameContext_getAudioSystem(context);
    AudioSystem_stopGroup(audioSystem, GAME_AUDIO_GROUP_MUSIC, 1000);

    MIX_Audio* music = AssetManager_getAudio(assets, AUDIO_MUSIC_1);
    AudioPlayOptions options = { 0 };
    AudioPlayOptions_init(&options);
    options.startTimeMS = 0;
    options.loopCount = -1;
    AudioSystem_play(audioSystem, GAME_AUDIO_GROUP_MUSIC, music, &options);

    GameSpriteId layerIds[] = {
        SPRITE_BACK_LAYER_0_A,
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
            Vec2_set(-0.1f * i - 0.0f, 0.f),
            spriteGroup, 0, i
        );
    }

    Timer_setTimeScale(g_engine.time, 1.f);
}

static void TitleScene_onQuit(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    TitleScene* self = (TitleScene*)selfPtr;
    GameContext* context = Scene_getContext(self);

    TitleUIManager_closePages(self->m_uiManager);
    TitleUIManager_update(self->m_uiManager);

    ObjectManager* objectManager = GameContext_getObjectManager(context);
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    ObjectManager_clear(objectManager);
    PhysicsEngine_clear(physicsEngine);
}

static void TitleScene_onUpdate(void* selfPtr, float dt)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    TitleScene* self = (TitleScene*)selfPtr;

    TitleUIManager_update(self->m_uiManager);
}

static void TitleScene_onFixedUpdate(void* selfPtr, float timeStep)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    TitleScene* self = (TitleScene*)selfPtr;
}

static void TitleScene_onDestroy(void* selfPtr)
{
    assert(selfPtr && "self must not be NULL");
    assert(Scene_isValid(selfPtr) && "self must be a valid Scene");
    TitleScene* self = (TitleScene*)selfPtr;
    TitleUIManager_destroy(self->m_uiManager);
}
