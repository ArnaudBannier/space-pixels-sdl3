/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/core/game_object.h"
#include "game/core/game_context.h"

static void GameObjectVM_onRender(void* selfPtr)
{
    assert(false && "onRender must be override");
}

static void GameObjectVM_onUpdate(void* selfPtr, float dt)
{
    assert(false && "onUpdate must be override");
}

static void GameObjectVM_onFixedUpdate(void* selfPtr, float timeStep)
{
    assert(false && "onFixedUpdate must be override");
}

static void GameObjectVM_onDestroy(void* selfPtr)
{
    assert(false && "onDestroy must be override");
}

static void GameObjectVM_onDrawGizmos(void* selfPtr)
{
    assert(false && "onDrawGizmos must be override");
}

static void GameObjectVM_onTakeDamage(void* selfPtr, float damage, int playerId)
{
    assert(selfPtr && "self must not be NULL");
    printf("GameObject took damage, but onTakeDamage is not implemented for this object type.\n");
}

void GameObject_init(void* selfPtr, GameContext* context, int type)
{
    GameObject* self = (GameObject*)selfPtr;
    assert(self && "self must not be NULL");

    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    AssetManager* assets = GameContext_getAssetManager(context);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);

    self->m_magicNumber = MAGIC_NUMBER_GAME_OBJECT;
    self->m_type = type;
    self->m_context = context;
    self->m_shouldBeDestroy = false;

    BodyId bodyId = PhysicsEngine_createBody(physicsEngine, type, self);
    Body* body = PhysicsEngine_getBody(physicsEngine, bodyId);
    body->colliderRadius = 0.5f;
    FixedUpdateTransform_set(
        &(body->transform),
        TRANSFORM_SPACE_WORLD,
        Vec2_zero,
        0.0f
    );

    self->m_bodyId = bodyId;
    self->m_onDestroy = GameObjectVM_onDestroy;
    self->m_onFixedUpdate = GameObjectVM_onFixedUpdate;
    self->m_onUpdate = GameObjectVM_onUpdate;
    self->m_onRender = GameObjectVM_onRender;
    self->m_onDrawGizmos = GameObjectVM_onDrawGizmos;
    self->m_onTakeDamage = GameObjectVM_onTakeDamage;

    GameContext_addGameObject(context, self);
}

void GameObject_destroy(void* selfPtr)
{
    GameObject* self = (GameObject*)selfPtr;
    if (!self) return;

    self->m_onDestroy(self);

    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(self->m_context);
    PhysicsEngine_destroyBody(physicsEngine, self->m_bodyId);

    free(self);
}

void GameObject_update(void* selfPtr, float dt)
{
    GameObject* self = (GameObject*)selfPtr;
    assert(GameObject_isValid(self) && "self must be a valid GameObject");
    self->m_onUpdate(self, dt);
}

void GameObject_fixedUpdate(void* selfPtr, float timeStep)
{
    GameObject* self = (GameObject*)selfPtr;
    assert(GameObject_isValid(self) && "self must be a valid GameObject");
    self->m_onFixedUpdate(self, timeStep);
}

void GameObject_render(void* selfPtr)
{
    GameObject* self = (GameObject*)selfPtr;
    assert(GameObject_isValid(self) && "self must be a valid GameObject");
    self->m_onRender(self);
}

void GameObject_drawGizmos(void* selfPtr)
{
    GameObject* self = (GameObject*)selfPtr;
    assert(GameObject_isValid(self) && "self must be a valid GameObject");
    self->m_onDrawGizmos(self);
}

void GameObject_takeDamage(void* selfPtr, float damage, int playerId)
{
    GameObject* self = (GameObject*)selfPtr;
    assert(GameObject_isValid(self) && "self must be a valid GameObject");
    self->m_onTakeDamage(self, damage, playerId);
}

bool GameObject_isValid(void* selfPtr)
{
    if (!selfPtr) return false;
    if (((GameObject*)selfPtr)->m_magicNumber != MAGIC_NUMBER_GAME_OBJECT) return false;
    return true;
}

Body* GameObject_getBody(void* selfPtr)
{
    GameObject* self = (GameObject*)selfPtr;
    assert(GameObject_isValid(self) && "self must be a valid GameObject");
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(self->m_context);
    return PhysicsEngine_getBody(physicsEngine, self->m_bodyId);
}

Transform* GameObject_getTransform(void* selfPtr)
{
    GameObject* self = (GameObject*)selfPtr;
    assert(GameObject_isValid(self) && "self must be a valid GameObject");
    return &(self->m_transform);
}

int GameObject_getType(void* selfPtr)
{
    GameObject* self = (GameObject*)selfPtr;
    assert(GameObject_isValid(self) && "self must be a valid GameObject");
    return self->m_type;
}

void GameObject_updateTransform(void* selfPtr)
{
    GameObject* self = (GameObject*)selfPtr;
    assert(GameObject_isValid(self) && "self must be a valid GameObject");
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(self->m_context);
    Body* body = PhysicsEngine_getBody(physicsEngine, self->m_bodyId);
    const float stepAlpha = GameContext_getStepAlpha(self->m_context);

    Transform_setFromFixedUpdate(
        &self->m_transform,
        &body->transform,
        stepAlpha
    );
}
