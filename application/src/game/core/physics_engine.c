/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/core/physics_engine.h"

static void BodyManager_init(BodyManager* self, int capacity)
{
    self->m_capacity = capacity;
    self->m_bodyCount = 0;
    self->m_bodies = (Body*)calloc(capacity, sizeof(Body));
    ASSERT_NEW(self->m_bodies);

    self->m_generations = (uint32_t*)calloc(capacity, sizeof(uint32_t));
    ASSERT_NEW(self->m_generations);

    self->m_sparseToDense = (int*)calloc(capacity, sizeof(int));
    ASSERT_NEW(self->m_sparseToDense);

    self->m_denseToSparse = (int*)calloc(capacity, sizeof(int));
    ASSERT_NEW(self->m_denseToSparse);

    self->m_nextFree = (int*)calloc(capacity, sizeof(int));
    ASSERT_NEW(self->m_nextFree);

    for (int i = 0; i < capacity; i++)
    {
        self->m_generations[i] = 1;
    }

    // Initialize the free list
    for (int i = 0; i < capacity - 1; i++)
    {
        self->m_nextFree[i] = i + 1;
    }
    self->m_nextFree[capacity - 1] = -1; // End of free list
    self->m_firstFreeIndex = 0;
}

static void BodyManager_destroy(BodyManager* self)
{
    free(self->m_bodies);
    free(self->m_generations);
    free(self->m_sparseToDense);
    free(self->m_denseToSparse);
    free(self->m_nextFree);
}

static void BodyManager_clear(BodyManager* self)
{
    self->m_bodyCount = 0;
    // Increment generations to invalidate all existing BodyIds
    for (int i = 0; i < self->m_capacity; i++)
    {
        self->m_generations[i]++;
    }
    // Reinitialize the free list
    for (int i = 0; i < self->m_capacity - 1; i++)
    {
        self->m_nextFree[i] = i + 1;
    }
    self->m_nextFree[self->m_capacity - 1] = -1;
    self->m_firstFreeIndex = 0;
}

static BodyId BodyManager_createBody(BodyManager* self, int userType, void* userData)
{
    if (self->m_firstFreeIndex == -1)
    {
        assert(false && "BodyManager capacity exceeded");
        return BodyId_null;
    }

    int sparseIndex = self->m_firstFreeIndex;
    int denseIndex = self->m_bodyCount++;
    self->m_firstFreeIndex = self->m_nextFree[sparseIndex];

    self->m_sparseToDense[sparseIndex] = denseIndex;
    self->m_denseToSparse[denseIndex] = sparseIndex;

    Body* body = &self->m_bodies[denseIndex];
    memset(body, 0, sizeof(Body));
    body->userType = userType;
    body->userData = userData;

    BodyId id = {
        .index = (uint32_t)sparseIndex,
        .generation = self->m_generations[sparseIndex]
    };

    return id;
}

static void BodyManager_destroyBody(BodyManager* self, BodyId id)
{
    if (id.index >= (uint32_t)self->m_capacity) return;
    if (self->m_generations[id.index] != id.generation) return;

    int denseIndex = self->m_sparseToDense[id.index];
    int lastDenseIndex = self->m_bodyCount - 1;
    if (denseIndex != lastDenseIndex)
    {
        // Move the last body to the freed slot
        self->m_bodies[denseIndex] = self->m_bodies[lastDenseIndex];
        // Update mappings
        int movedSparseIndex = self->m_denseToSparse[lastDenseIndex];
        self->m_sparseToDense[movedSparseIndex] = denseIndex;
        self->m_denseToSparse[denseIndex] = movedSparseIndex;
    }

    self->m_bodyCount--;

    // Add the freed slot back to the free list
    self->m_nextFree[id.index] = self->m_firstFreeIndex;
    self->m_firstFreeIndex = id.index;

    // Increment the generation to invalidate old IDs
    self->m_generations[id.index]++;
}

static Body* BodyManager_getBody(BodyManager* self, BodyId id)
{
    assert(self && "self must not be NULL");
    if (id.index >= (uint32_t)self->m_capacity) return NULL;
    if (self->m_generations[id.index] != id.generation) return NULL;

    int denseIndex = self->m_sparseToDense[id.index];
    return &self->m_bodies[denseIndex];
}

void Body_setTargetPosition(Body* self, Vec2 position, float timeStep)
{
    assert(self && "self must not be NULL");
    if (timeStep > 1e-6f)
    {
        self->velocity = Vec2_scale(
            Vec2_sub(position, self->transform.currPosition),
            1.f / timeStep
        );
    }
    else
    {
        assert(false);
        self->velocity = Vec2_zero;
        self->transform.currPosition = position;
        self->transform.prevPosition = position;
    }
}

PhysicsEngine* PhysicsEngine_create(int capacity)
{
    PhysicsEngine* self = (PhysicsEngine*)calloc(1, sizeof(PhysicsEngine));
    ASSERT_NEW(self);

    BodyManager_init(&self->m_bodyManager, capacity);

    self->m_broadPhaseRanges = (BPRange*)calloc(capacity, sizeof(BPRange));
    ASSERT_NEW(self->m_broadPhaseRanges);

    return self;
}

void PhysicsEngine_destroy(PhysicsEngine* self)
{
    if (!self) return;
    BodyManager_destroy(&self->m_bodyManager);
    free(self);
}

void PhysicsEngine_clear(PhysicsEngine* self)
{
    assert(self && "self must not be NULL");
    BodyManager_clear(&self->m_bodyManager);
}

BodyId PhysicsEngine_createBody(PhysicsEngine* self, int userType, void* userData)
{
    assert(self && "self must not be NULL");
    return BodyManager_createBody(&self->m_bodyManager, userType, userData);
}

void PhysicsEngine_destroyBody(PhysicsEngine* self, BodyId id)
{
    assert(self && "self must not be NULL");
    BodyManager_destroyBody(&self->m_bodyManager, id);
}

Body* PhysicsEngine_getBody(PhysicsEngine* self, BodyId id)
{
    assert(self && "self must not be NULL");
    return BodyManager_getBody(&self->m_bodyManager, id);
}

static int BPRange_compare(const void* ptrA, const void* ptrB)
{
    const BPRange* rangeA = (const BPRange*)ptrA;
    const BPRange* rangeB = (const BPRange*)ptrB;
    if (rangeA->min > rangeB->min) return +1;
    if (rangeA->min < rangeB->min) return -1;
    return 0;
}

static bool Body_isColliding(const Body* bodyA, const Body* bodyB)
{
    if ((bodyA->maskBits & bodyB->categoryBits) == 0) return false;
    if ((bodyB->maskBits & bodyA->categoryBits) == 0) return false;

    const Vec2 delta = Vec2_sub(bodyA->transform.currPosition, bodyB->transform.currPosition);
    const float distanceSquared = Vec2_lengthSquared(delta);
    const float radiusSum = bodyA->colliderRadius + bodyB->colliderRadius;
    return distanceSquared <= radiusSum * radiusSum;
}

static float Body_collide(const Body* bodyA, const Body* bodyB)
{
    const Vec2 delta = Vec2_sub(bodyA->transform.currPosition, bodyB->transform.currPosition);
    const float radiusSum = bodyA->colliderRadius + bodyB->colliderRadius;
    return Vec2_length(delta) - radiusSum;
}

void PhysicsEngine_fixedUpdate(PhysicsEngine* self, float timeStep)
{
    BodyManager* bodyManager = &self->m_bodyManager;
    const int bodyCount = bodyManager->m_bodyCount;
    for (int i = 0; i < bodyCount; i++)
    {
        Body* body = &bodyManager->m_bodies[i];
        FixedUpdateTransform* transform = &body->transform;
        transform->prevPosition = transform->currPosition;
        transform->prevAngle = transform->currAngle;
        transform->currPosition = Vec2_add(
            transform->prevPosition,
            Vec2_scale(body->velocity, timeStep)
        );
        transform->currAngle = transform->prevAngle + body->angularVelocity * timeStep;
    }

    // Update broad phase ranges
    for (int i = 0; i < bodyCount; i++)
    {
        Body* body = &bodyManager->m_bodies[i];
        BPRange* range = &self->m_broadPhaseRanges[i];
        body->collisionCount = 0;

        range->denseIndex = i;
        range->min = body->transform.currPosition.x - body->colliderRadius;
        range->max = body->transform.currPosition.x + body->colliderRadius;
    }

    // Sort the broad phase ranges by their minimum value
    qsort(self->m_broadPhaseRanges, bodyManager->m_bodyCount, sizeof(BPRange), BPRange_compare);

    // Check for collisions using the broad phase ranges
    for (int i = 0; i < bodyCount; i++)
    {
        const BPRange* rangeA = &self->m_broadPhaseRanges[i];
        const int denseIndexA = rangeA->denseIndex;
        Body* bodyA = &bodyManager->m_bodies[denseIndexA];

        for (int j = i + 1; j < bodyCount; j++)
        {
            const BPRange* rangeB = &self->m_broadPhaseRanges[j];
            if (rangeA->max < rangeB->min) break;

            const int denseIndexB = rangeB->denseIndex;
            Body* bodyB = &bodyManager->m_bodies[denseIndexB];
            if (!Body_isColliding(bodyA, bodyB)) continue;

            const float penetration = -Body_collide(bodyA, bodyB);
            const int sparseIndexA = bodyManager->m_denseToSparse[denseIndexA];
            const int sparseIndexB = bodyManager->m_denseToSparse[denseIndexB];

            BodyId idA = {
                .index = sparseIndexA,
                .generation = bodyManager->m_generations[sparseIndexA]
            };
            BodyId idB = {
                .index = sparseIndexB,
                .generation = bodyManager->m_generations[sparseIndexB]
            };

            if (bodyA->collisionCount < MAX_COLLISION_COUNT)
            {
                Collision collisionA = { .other = idB, .penetration = penetration };
                bodyA->collisions[bodyA->collisionCount++] = collisionA;
            }
            if (bodyB->collisionCount < MAX_COLLISION_COUNT)
            {
                Collision collisionB = { .other = idA, .penetration = penetration };
                bodyB->collisions[bodyB->collisionCount++] = collisionB;
            }
        }
    }
}

int PhysicsEngine_queryCircle(PhysicsEngine* self, Vec2 center, float radius, uint32_t maskBits, BodyId* outBodies, int maxCount)
{
    BPRange queryRange = {
        .min = center.x - radius,
        .max = center.x + radius
    };

    BodyManager* bodyManager = &self->m_bodyManager;
    const int bodyCount = bodyManager->m_bodyCount;
    if (bodyCount == 0 || maxCount == 0) return 0;

    // Dichotomic search to find the starting index of ranges that could overlap with the query range
    int left = 0;
    int right = bodyCount;
    while (left < right)
    {
        int mid = left + (right - left) / 2;
        if (self->m_broadPhaseRanges[mid].min <= queryRange.max)
        {
            left = mid + 1;
        }
        else
        {
            right = mid;
        }
    }
    int startIndex = left;

    // Reculer pour capturer les ranges dont le max pourrait chevaucher
    while (startIndex > 0 && self->m_broadPhaseRanges[startIndex - 1].max >= queryRange.min)
    {
        startIndex--;
    }

    // Parcourir les ranges et vérifier les collisions avec le cercle
    int count = 0;
    const float radiusSquared = radius * radius;
    for (int i = startIndex; i < bodyCount && count < maxCount; i++)
    {
        const BPRange* currRange = &self->m_broadPhaseRanges[i];
        if (currRange->min > queryRange.max) break;

        const int denseIndex = currRange->denseIndex;
        const Body* body = &bodyManager->m_bodies[denseIndex];

        if ((body->categoryBits & maskBits) == 0) continue;

        // Vérifier si le corps est dans le cercle
        const Vec2 delta = Vec2_sub(body->transform.currPosition, center);
        const float distSquared = Vec2_lengthSquared(delta);
        const float radiusSum = radius + body->colliderRadius;

        if (distSquared <= radiusSum * radiusSum)
        {
            const int sparseIndex = bodyManager->m_denseToSparse[denseIndex];
            BodyId id = {
                .index = sparseIndex,
                .generation = bodyManager->m_generations[sparseIndex]
            };
            outBodies[count++] = id;
        }
    }

    return count;
}
