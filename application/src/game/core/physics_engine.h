/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "game/core/core_common.h"

typedef struct Collision
{
    float penetration;
    BodyId other;
} Collision;

#define MAX_COLLISION_COUNT 4

typedef struct Body
{
    FixedUpdateTransform transform;
    Vec2 velocity;
    float angularVelocity;
    float colliderRadius;

    Collision collisions[MAX_COLLISION_COUNT];
    int collisionCount;

    uint32_t maskBits;
    uint32_t categoryBits;

    int userType;
    void* userData;
} Body;

void Body_setTargetPosition(Body* self, Vec2 position, float timeStep);

typedef struct BodyManager
{
    /// @brief Pointer to an array of bodies.
    /// This is the dense array where active bodies are stored.
    Body* m_bodies;

    /// @brief Pointer to an array of generation counters for each body slot.
    /// Used to validate BodyIds. When a body is destroyed, its generation is incremented to invalidate old IDs.
    /// This array is indexed by the sparse index from BodyId.
    uint32_t* m_generations;

    /// @brief Pointer to an array mapping sparse indices (from BodyId) to dense indices (in m_bodies).
    int* m_sparseToDense;

    /// @brief Pointer to an array mapping dense indices (in m_bodies) back to sparse indices (for BodyId).
    int* m_denseToSparse;

    /// @brief The current number of active bodies in the physics engine.
    /// This is also the next dense index to use when creating a new body.
    int m_bodyCount;

    /// @brief The maximum capacity of bodies that the physics engine can handle.
    int m_capacity;

    /// @brief The head of the free list for body slots.
    /// This is an index into the arrays that indicates the next free slot.
    int m_firstFreeIndex;

    /// @brief Pointer to an array that forms a linked list of free sparse indices.
    /// When a body is destroyed, its sparse index is added to this free list.
    int* m_nextFree;
} BodyManager;

typedef struct BPRange
{
    int denseIndex;
    float min;
    float max;
} BPRange;

typedef struct PhysicsEngine
{
    BodyManager m_bodyManager;
    BPRange* m_broadPhaseRanges;
} PhysicsEngine;

PhysicsEngine* PhysicsEngine_create(int capacity);
void PhysicsEngine_destroy(PhysicsEngine* self);
void PhysicsEngine_clear(PhysicsEngine* self);
void PhysicsEngine_fixedUpdate(PhysicsEngine* self, float timeStep);
BodyId PhysicsEngine_createBody(PhysicsEngine* self, int userType, void* userData);
void PhysicsEngine_destroyBody(PhysicsEngine* self, BodyId id);
Body* PhysicsEngine_getBody(PhysicsEngine* self, BodyId id);
int PhysicsEngine_queryCircle(PhysicsEngine* self, Vec2 center, float radius, uint32_t maskBits, BodyId* outBodies, int maxCount);
