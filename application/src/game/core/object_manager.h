/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "game/core/core_common.h"

typedef struct ObjectManager
{
    /// @brief Pointer to an array of game objects.
    /// This is the dense array where active objects are stored.
    GameObject** m_objects;

    /// @brief Pointer to an array of generation counters for each object slot.
    /// Used to validate ObjectIds. When an object is destroyed, its generation is incremented to invalidate old IDs.
    /// This array is indexed by the sparse index from ObjectId.
    uint32_t* m_generations;

    /// @brief Pointer to an array mapping sparse indices (from ObjectId) to dense indices (in m_objects).
    int* m_sparseToDense;

    /// @brief Pointer to an array mapping dense indices (in m_objects) back to sparse indices (for ObjectId).
    int* m_denseToSparse;

    /// @brief The current number of active objects in the object manager.
    /// This is also the next dense index to use when creating a new object.
    int m_objectCount;

    /// @brief The maximum capacity of objects that the object manager can handle.
    int m_capacity;

    /// @brief The head of the free list for object slots.
    /// This is an index into the arrays that indicates the next free slot.
    int m_firstFreeIndex;

    /// @brief Pointer to an array that forms a linked list of free sparse indices.
    /// When an object is destroyed, its sparse index is added to this free list.
    int* m_nextFree;

    /// @brief Stack of objects that should be destroyed at the end of the update loop.
    ObjectId* m_toDestroy;

    /// @brief The current number of objects in the m_toDestroy stack.
    int m_toDestroyCount;
} ObjectManager;

ObjectManager* ObjectManager_create(int capacity);
void ObjectManager_destroy(ObjectManager* self);
void ObjectManager_clear(ObjectManager* self);
ObjectId ObjectManager_addObject(ObjectManager* self, GameObject* object);
GameObject* ObjectManager_getObject(ObjectManager* self, ObjectId id);
void ObjectManager_processObjects(ObjectManager* self);

INLINE GameObject** ObjectManager_getObjects(ObjectManager* self)
{
    assert(self && "self must not be NULL");
    return self->m_objects;
}

INLINE int ObjectManager_getObjectCount(ObjectManager* self)
{
    assert(self && "self must not be NULL");
    return self->m_objectCount;
}

