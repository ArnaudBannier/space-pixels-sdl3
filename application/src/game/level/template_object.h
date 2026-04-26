/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#pragma once

#include "settings.h"
#include "common/common.h"
#include "game/core/game_object.h"

typedef struct TemplateObject
{
    GameObject m_base;
} TemplateObject;

TemplateObject* TemplateObject_create(GameContext* context, Vec2 position);
