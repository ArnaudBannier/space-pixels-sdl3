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
#include "game/core/object_manager.h"

typedef struct Scene Scene;
typedef struct GameContext GameContext;

typedef struct GameObject
{
    uint64_t m_magicNumber;
    ObjectId m_id;
    int m_type;
    GameContext* m_context;
    BodyId m_bodyId;
    Transform m_transform;
    bool m_shouldBeDestroy;

    void (*m_onRender)(void* selfPtr);
    void (*m_onUpdate)(void* selfPtr, float dt);
    void (*m_onFixedUpdate)(void* selfPtr, float timeStep);
    void (*m_onDrawGizmos)(void* selfPtr);
    void (*m_onDestroy)(void* selfPtr);
    void (*m_onTakeDamage)(void* selfPtr, float damage, int playerId);
} GameObject;

void GameObject_init(
    void* selfPtr, GameContext* context, int type);

void GameObject_destroy(void* selfPtr);
void GameObject_update(void* selfPtr, float dt);
void GameObject_fixedUpdate(void* selfPtr, float timeStep);
void GameObject_render(void* selfPtr);
void GameObject_drawGizmos(void* selfPtr);
void GameObject_takeDamage(void* selfPtr, float damage, int playerId);
bool GameObject_isValid(void* selfPtr);

INLINE void GameObject_setToDestroy(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    ((GameObject*)selfPtr)->m_shouldBeDestroy = true;
}

void GameObject_updateTransform(void* selfPtr);

Body* GameObject_getBody(void* selfPtr);
Transform* GameObject_getTransform(void* selfPtr);
int GameObject_getType(void* selfPtr);

INLINE ObjectId GameObject_getId(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    return ((GameObject*)selfPtr)->m_id;
}

INLINE GameContext* GameObject_getContext(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    return ((GameObject*)selfPtr)->m_context;
}

INLINE bool GameObject_shouldBeDestroy(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    return ((GameObject*)selfPtr)->m_shouldBeDestroy;
}
