/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"
#include "game/core/game_object.h"

typedef enum ItemType
{
    ITEM_TYPE_NONE,
    ITEM_TYPE_CURE,
} ItemType;

typedef struct Item
{
    GameObject m_base;
    ItemType m_type;
    SpriteAnimState m_mainAnim;
    RenderDim m_renderDim;
} Item;

Item* Item_create(GameContext* context, ItemType type, Vec2 position);
