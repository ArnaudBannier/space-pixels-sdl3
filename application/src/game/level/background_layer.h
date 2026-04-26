/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"
#include "game/core/game_object.h"

typedef struct BackgroundLayer
{
    GameObject m_base;
    RenderDim m_renderDim;
    Vec2 m_parallaxSpeed;
    Vec2 m_worldDimensions;
    RenderSortingLayer m_sortingLayer;
    SpriteGroup* m_spriteGroup;
    int m_spriteIndex;
    int m_orderInLayer;
} BackgroundLayer;

BackgroundLayer* BackgroundLayer_create(
    GameContext* context,
    Vec2 worldDimensions, Vec2 parallaxSpeed,
    SpriteGroup* spriteGroup, int spriteIndex, int orderInLayer);
