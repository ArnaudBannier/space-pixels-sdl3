/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/level/template_enemy.h"
#include "game/core/game_context.h"
#include "game/level/linear_bullet.h"

static void TemplateEnemyVM_onDestroy(void* selfPtr);
static void TemplateEnemyVM_onUpdate(void* selfPtr, float dt);
static void TemplateEnemyVM_onFixedUpdate(void* selfPtr, float timeStep);
static void TemplateEnemyVM_onRender(void* selfPtr);
static void TemplateEnemyVM_onDrawGizmos(void* selfPtr);
static void TemplateEnemyVM_onTakeDamage(void* selfPtr, float damage, int playerId);

TemplateEnemy* TemplateEnemy_create(GameContext* context, Vec2 position, ItemType itemType)
{
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    AssetManager* assets = GameContext_getAssetManager(context);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);

    TemplateEnemy* self = (TemplateEnemy*)calloc(1, sizeof(TemplateEnemy));
    ASSERT_NEW(self);

    GameObject_init(self, context, GAME_TYPE_UNDEFINED);

    EnemyData* enemyData = &self->m_enemyData;
    enemyData->idleAnimId = ANIM_PLAYER_ENGINE;
    enemyData->firingAnimId = ANIM_PLAYER_ENGINE;
    enemyData->dyingAnimId = ANIM_PLAYER_ENGINE;
    enemyData->shieldAnimId = ANIM_PLAYER_ENGINE;
    enemyData->hp = 100.f;
    enemyData->points = 25.f;
    enemyData->position = position;
    enemyData->itemType = itemType;
    Enemy_init(self, &self->m_enemyData);

    Body* body = GameObject_getBody(self);
    body->colliderRadius = 1.0f;

    GameObject* selfObj = (GameObject*)self;
    selfObj->m_onDestroy = TemplateEnemyVM_onDestroy;
    selfObj->m_onDrawGizmos = TemplateEnemyVM_onDrawGizmos;
    selfObj->m_onFixedUpdate = TemplateEnemyVM_onFixedUpdate;
    selfObj->m_onRender = TemplateEnemyVM_onRender;
    selfObj->m_onUpdate = TemplateEnemyVM_onUpdate;
    selfObj->m_onTakeDamage = TemplateEnemyVM_onTakeDamage;

    TemplateEnemyVM_onFixedUpdate(self, 0.f);
    TemplateEnemyVM_onUpdate(self, 0.f);

    return self;
}

static void TemplateEnemyVM_onDestroy(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    TemplateEnemy* self = (TemplateEnemy*)selfPtr;

    EnemyData* enemyData = &self->m_enemyData;
    Enemy_onDestroy(self, enemyData);
}

static void TemplateEnemyVM_onUpdate(void* selfPtr, float dt)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    TemplateEnemy* self = (TemplateEnemy*)selfPtr;

    EnemyData* enemyData = &self->m_enemyData;
    Enemy_onUpdate(self, enemyData, dt);
}

static void TemplateEnemyVM_onFixedUpdate(void* selfPtr, float timeStep)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    TemplateEnemy* self = (TemplateEnemy*)selfPtr;

    EnemyData* enemyData = &self->m_enemyData;
    Enemy_onFixedUpdate(self, enemyData, timeStep);
}

static void TemplateEnemyVM_onRender(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    TemplateEnemy* self = (TemplateEnemy*)selfPtr;

    EnemyData* enemyData = &self->m_enemyData;
    Enemy_onRender(self, enemyData);
}

static void TemplateEnemyVM_onDrawGizmos(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    TemplateEnemy* self = (TemplateEnemy*)selfPtr;

    EnemyData* enemyData = &self->m_enemyData;
    Enemy_onDrawGizmos(self, enemyData, &(g_colors.pink5));
}

static void TemplateEnemyVM_onTakeDamage(void* selfPtr, float damage, int playerId)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    TemplateEnemy* self = (TemplateEnemy*)selfPtr;

    EnemyData* enemyData = &self->m_enemyData;
    Enemy_onTakeDamage(self, enemyData, damage, playerId);
}
