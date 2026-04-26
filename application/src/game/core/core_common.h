/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"

typedef struct ObjectId
{
    uint32_t index;
    uint32_t generation;
} ObjectId;

typedef struct BodyId
{
    uint32_t index;
    uint32_t generation;
} BodyId;

extern const ObjectId ObjectId_null;
extern const BodyId BodyId_null;

INLINE bool ObjectId_isNull(ObjectId id)
{
    return id.generation == 0;
}

INLINE bool BodyId_isNull(BodyId id)
{
    return id.generation == 0;
}

typedef struct GameObject GameObject;
