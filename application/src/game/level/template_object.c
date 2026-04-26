/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/level/template_object.h"
#include "game/core/game_context.h"

static void TemplateObjectVM_onDestroy(void* selfPtr);
static void TemplateObjectVM_onUpdate(void* selfPtr, float dt);
static void TemplateObjectVM_onFixedUpdate(void* selfPtr, float timeStep);
static void TemplateObjectVM_onRender(void* selfPtr);
static void TemplateObjectVM_onDrawGizmos(void* selfPtr);
static void TemplateObjectVM_onTakeDamage(void* selfPtr, float damage, int playerId);

TemplateObject* TemplateObject_create(GameContext* context, Vec2 position)
{
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    AssetManager* assets = GameContext_getAssetManager(context);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);

    TemplateObject* self = (TemplateObject*)calloc(1, sizeof(TemplateObject));
    ASSERT_NEW(self);

    GameObject_init(self, context, GAME_TYPE_UNDEFINED);

    Body* body = GameObject_getBody(self);
    body->colliderRadius = 1.0f;
    body->categoryBits = GAME_CATEGORY_ENEMY;
    body->maskBits = GAME_CATEGORY_PLAYER_BULLET;
    FixedUpdateTransform_set(
        &(body->transform),
        TRANSFORM_SPACE_WORLD,
        Vec2_set(0.f, 0.f),
        +90.0f
    );

    GameObject* selfObj = (GameObject*)self;
    selfObj->m_onDestroy = TemplateObjectVM_onDestroy;
    selfObj->m_onDrawGizmos = TemplateObjectVM_onDrawGizmos;
    selfObj->m_onFixedUpdate = TemplateObjectVM_onFixedUpdate;
    selfObj->m_onRender = TemplateObjectVM_onRender;
    selfObj->m_onUpdate = TemplateObjectVM_onUpdate;
    selfObj->m_onTakeDamage = TemplateObjectVM_onTakeDamage;

    TemplateObjectVM_onFixedUpdate(self, 0.f);
    TemplateObjectVM_onUpdate(self, 0.f);

    return self;
}

static void TemplateObjectVM_onDestroy(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    TemplateObject* self = (TemplateObject*)selfPtr;
}

static void TemplateObjectVM_onUpdate(void* selfPtr, float dt)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    TemplateObject* self = (TemplateObject*)selfPtr;
}

static void TemplateObjectVM_onFixedUpdate(void* selfPtr, float timeStep)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    TemplateObject* self = (TemplateObject*)selfPtr;
}

static void TemplateObjectVM_onRender(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    TemplateObject* self = (TemplateObject*)selfPtr;
}

static void TemplateObjectVM_onDrawGizmos(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    TemplateObject* self = (TemplateObject*)selfPtr;
}

static void TemplateObjectVM_onTakeDamage(void* selfPtr, float damage, int playerId)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    TemplateObject* self = (TemplateObject*)selfPtr;
}
