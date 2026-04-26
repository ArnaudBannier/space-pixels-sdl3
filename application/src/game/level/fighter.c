/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/level/fighter.h"
#include "game/core/game_context.h"
#include "game/level/linear_bullet.h"

static void FighterVM_onDestroy(void* selfPtr);
static void FighterVM_onUpdate(void* selfPtr, float dt);
static void FighterVM_onFixedUpdate(void* selfPtr, float timeStep);
static void FighterVM_onRender(void* selfPtr);
static void FighterVM_onDrawGizmos(void* selfPtr);
static void FighterVM_onTakeDamage(void* selfPtr, float damage, int playerId);

Fighter* Fighter_create(GameContext* context, Vec2 position, ItemType itemType)
{
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    AssetManager* assets = GameContext_getAssetManager(context);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);

    Fighter* self = (Fighter*)calloc(1, sizeof(Fighter));
    ASSERT_NEW(self);

    GameObject_init(self, context, GAME_TYPE_FIGHTER);

    // [TODO fighter]
    // - Attribuer les bonnes animations à l'ennemi

    EnemyData* enemyData = &self->m_enemyData;
    enemyData->idleAnimId = ANIM_PLAYER_ENGINE;
    enemyData->firingAnimId = ANIM_PLAYER_ENGINE;
    enemyData->dyingAnimId = ANIM_PLAYER_ENGINE;
    enemyData->shieldAnimId = ANIM_PLAYER_ENGINE;
    enemyData->hp = 10.f;
    enemyData->points = 3.f;
    enemyData->position = position;
    enemyData->itemType = itemType;
    Enemy_init(self, &self->m_enemyData);

    // [TODO enemy guizmos]
    // - Ajustez le collider de l'ennemi pour qu'il corresponde mieux à sa taille

    Body* body = GameObject_getBody(self);
    body->colliderRadius = 2.0f;

    GameObject* selfObj = (GameObject*)self;
    selfObj->m_onDestroy = FighterVM_onDestroy;
    selfObj->m_onDrawGizmos = FighterVM_onDrawGizmos;
    selfObj->m_onFixedUpdate = FighterVM_onFixedUpdate;
    selfObj->m_onRender = FighterVM_onRender;
    selfObj->m_onUpdate = FighterVM_onUpdate;
    selfObj->m_onTakeDamage = FighterVM_onTakeDamage;

    FighterVM_onFixedUpdate(self, 0.1f);
    FighterVM_onUpdate(self, 0.1f);

    return self;
}

static void FighterVM_onDestroy(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Fighter* self = (Fighter*)selfPtr;

    EnemyData* enemyData = &self->m_enemyData;
    Enemy_onDestroy(self, enemyData);
}

static void FighterVM_onUpdate(void* selfPtr, float dt)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Fighter* self = (Fighter*)selfPtr;

    EnemyData* enemyData = &self->m_enemyData;
    Enemy_onUpdate(self, enemyData, dt);

    if (enemyData->state != ENEMY_STATE_FIRING) return;

    GameContext* context = GameObject_getContext(self);
    Transform* transform = GameObject_getTransform(self);
    SpriteAnimState* mainAnim = &(enemyData->m_mainAnim);

    for (int i = 0; i < mainAnim->eventCount; i++)
    {
        SpriteAnimEvent* event = &mainAnim->events[i];
        if (event->type != ANIM_EVENT_FRAME_CHANGED) continue;

        const LinearBulletType bulletType = LINEAR_BULLET_TYPE_FIGHTER;

        // [TODO fighter shoot]
        // - Faites tirer l'ennemi à la frame 1

        //if (event->index == 1)
        //{
        //    Vec2 position = transform->position;
        //    position.x += -6.f * PIX_TO_WORLD;
        //    position.y += +6.f * PIX_TO_WORLD;

        //    (void)LinearBullet_create(context, bulletType, position, 3.5f, Vec2_left, -1);
        //    GameContext_playSpatialSFX(context, AUDIO_SFX_ENEMY_BASIC, transform);
        //}

        // [TODO fighter shoot]
        // - Faites tirer l'ennemi à la frame 3
        // - Trouvez la bonne position pour que les tirs partent du canon de l'ennemi
    }
}

static void FighterVM_onFixedUpdate(void* selfPtr, float timeStep)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Fighter* self = (Fighter*)selfPtr;
    Enemy_onFixedUpdate(self, &self->m_enemyData, timeStep);
}

static void FighterVM_onRender(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Fighter* self = (Fighter*)selfPtr;

    // [TODO fighter]
    // - Appeler Enemy_onRender pour dessiner l'ennemi

    //Enemy_onRender(self, &self->m_enemyData);
}

static void FighterVM_onDrawGizmos(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Fighter* self = (Fighter*)selfPtr;

    // [TODO enemy guizmos]
    // - Appeler Enemy_onDrawGizmos pour dessiner le collider de l'ennemi

    //Enemy_onDrawGizmos(self, &self->m_enemyData, &(g_colors.pink5));
}

static void FighterVM_onTakeDamage(void* selfPtr, float damage, int playerId)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Fighter* self = (Fighter*)selfPtr;
    Enemy_onTakeDamage(self, &self->m_enemyData, damage, playerId);
}
