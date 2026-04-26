/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/core/object_manager.h"
#include "game/core/game_object.h"

ObjectManager* ObjectManager_create(int capacity)
{
    ObjectManager* self = (ObjectManager*)calloc(1, sizeof(ObjectManager));
    ASSERT_NEW(self);

    self->m_objectCount = 0;
    self->m_capacity = capacity;
    self->m_objects = (GameObject**)calloc(capacity, sizeof(GameObject*));
    ASSERT_NEW(self->m_objects);

    self->m_generations = (uint32_t*)calloc(capacity, sizeof(uint32_t));
    ASSERT_NEW(self->m_generations);

    self->m_sparseToDense = (int*)calloc(capacity, sizeof(int));
    ASSERT_NEW(self->m_sparseToDense);

    self->m_denseToSparse = (int*)calloc(capacity, sizeof(int));
    ASSERT_NEW(self->m_denseToSparse);

    self->m_nextFree = (int*)calloc(capacity, sizeof(int));
    ASSERT_NEW(self->m_nextFree);

    self->m_toDestroyCount = 0;
    self->m_toDestroy = (ObjectId*)calloc(capacity, sizeof(ObjectId));
    ASSERT_NEW(self->m_toDestroy);

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

    return self;
}

void ObjectManager_destroy(ObjectManager* self)
{
    if (!self) return;

    free(self->m_objects);
    free(self->m_generations);
    free(self->m_sparseToDense);
    free(self->m_denseToSparse);
    free(self->m_nextFree);
    free(self->m_toDestroy);
    free(self);
}

void ObjectManager_clear(ObjectManager* self)
{
    assert(self && "self must not be NULL");
    const int objectCount = self->m_objectCount;
    for (int i = 0; i < objectCount; i++)
    {
        GameObject_destroy(self->m_objects[i]);
    }
    // After destroying all objects, we should have the same number of objects
    // We assert this to ensure that no object was added during the destruction process
    assert(self->m_objectCount == objectCount);

    self->m_objectCount = 0;
    self->m_toDestroyCount = 0;

    // Increment generations to invalidate all existing ObjectIds
    const int capacity = self->m_capacity;
    for (int i = 0; i < capacity; i++)
    {
        self->m_generations[i]++;
    }

    // Reinitialize the free list
    for (int i = 0; i < capacity - 1; i++)
    {
        self->m_nextFree[i] = i + 1;
    }
    self->m_nextFree[capacity - 1] = -1;
    self->m_firstFreeIndex = 0;
}

ObjectId ObjectManager_addObject(ObjectManager* self, GameObject* object)
{
    if (self->m_firstFreeIndex == -1)
    {
        assert(false && "ObjectManager capacity exceeded");
        return ObjectId_null;
    }

    int sparseIndex = self->m_firstFreeIndex;
    int denseIndex = self->m_objectCount++;
    self->m_firstFreeIndex = self->m_nextFree[sparseIndex];

    self->m_sparseToDense[sparseIndex] = denseIndex;
    self->m_denseToSparse[denseIndex] = sparseIndex;

    self->m_objects[denseIndex] = object;

    ObjectId id = {
        .index = (uint32_t)sparseIndex,
        .generation = self->m_generations[sparseIndex]
    };

    return id;
}

static void ObjectManager_destroyObject(ObjectManager* self, ObjectId id)
{
    if (id.index >= (uint32_t)self->m_capacity) return;
    if (self->m_generations[id.index] != id.generation) return;

    int denseIndex = self->m_sparseToDense[id.index];
    int lastDenseIndex = self->m_objectCount - 1;

    // Destroy the object
    GameObject_destroy(self->m_objects[denseIndex]);

    if (denseIndex != lastDenseIndex)
    {
        // Move the last body to the freed slot
        self->m_objects[denseIndex] = self->m_objects[lastDenseIndex];
        // Update mappings
        int movedSparseIndex = self->m_denseToSparse[lastDenseIndex];
        self->m_sparseToDense[movedSparseIndex] = denseIndex;
        self->m_denseToSparse[denseIndex] = movedSparseIndex;
    }

    self->m_objectCount--;

    // Add the freed slot back to the free list
    self->m_nextFree[id.index] = self->m_firstFreeIndex;
    self->m_firstFreeIndex = id.index;

    // Increment the generation to invalidate old IDs
    self->m_generations[id.index]++;
}

GameObject* ObjectManager_getObject(ObjectManager* self, ObjectId id)
{
    assert(self && "self must not be NULL");
    if (id.index >= (uint32_t)self->m_capacity) return NULL;
    if (self->m_generations[id.index] != id.generation) return NULL;

    int denseIndex = self->m_sparseToDense[id.index];
    return self->m_objects[denseIndex];
}

void ObjectManager_processObjects(ObjectManager* self)
{
    assert(self && "self must not be NULL");

    self->m_toDestroyCount = 0;
    for (int i = 0; i < self->m_objectCount; i++)
    {
        GameObject* object = self->m_objects[i];
        if (GameObject_shouldBeDestroy(object))
        {
            ObjectId id = GameObject_getId(object);
            self->m_toDestroy[self->m_toDestroyCount++] = id;
        }
    }

    for (int i = 0; i < self->m_toDestroyCount; i++)
    {
        ObjectId id = self->m_toDestroy[i];
        ObjectManager_destroyObject(self, id);
    }
}
