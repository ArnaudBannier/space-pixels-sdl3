/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/level/item.h"
#include "game/core/game_context.h"
#include "game/level/linear_bullet.h"
#include "game/level/player.h"

static void ItemVM_onDestroy(void* selfPtr);
static void ItemVM_onUpdate(void* selfPtr, float dt);
static void ItemVM_onFixedUpdate(void* selfPtr, float timeStep);
static void ItemVM_onRender(void* selfPtr);
static void ItemVM_onDrawGizmos(void* selfPtr);
static void ItemVM_onTakeDamage(void* selfPtr, float damage, int playerId);

Item* Item_create(GameContext* context, ItemType type, Vec2 position)
{
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    AssetManager* assets = GameContext_getAssetManager(context);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);

    Item* self = (Item*)calloc(1, sizeof(Item));
    ASSERT_NEW(self);

    GameObject_init(self, context, GAME_TYPE_ITEM);

    self->m_type = type;

    Body* body = GameObject_getBody(self);
    body->colliderRadius = 0.30f;
    body->categoryBits = GAME_CATEGORY_ITEM;
    body->maskBits = GAME_CATEGORY_PLAYER;
    FixedUpdateTransform_set(
        &(body->transform),
        TRANSFORM_SPACE_WORLD,
        position,
        0.0f
    );
    body->velocity = Vec2_set(-1.f, 0.f);

    switch (type)
    {
    case ITEM_TYPE_CURE:
        SpriteAnimState_init(&self->m_mainAnim, ANIM_ITEM_CURE);
        break;
    default:
    case ITEM_TYPE_NONE:
        assert(false);
        break;
    }
    RenderDim_setPixelsPerUnit(&self->m_renderDim, PIX_PER_UNIT * 2.f);

    GameObject* selfObj = (GameObject*)self;
    selfObj->m_onDestroy = ItemVM_onDestroy;
    selfObj->m_onDrawGizmos = ItemVM_onDrawGizmos;
    selfObj->m_onFixedUpdate = ItemVM_onFixedUpdate;
    selfObj->m_onRender = ItemVM_onRender;
    selfObj->m_onUpdate = ItemVM_onUpdate;
    selfObj->m_onTakeDamage = ItemVM_onTakeDamage;

    ItemVM_onFixedUpdate(self, 0.f);
    ItemVM_onUpdate(self, 0.f);

    return self;
}

static void ItemVM_onDestroy(void* selfPtr)
{
    // Nothing to do
}

static void ItemVM_onUpdate(void* selfPtr, float dt)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Item* self = (Item*)selfPtr;

    GameContext* context = GameObject_getContext(self);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);

    SpriteAnimManager_updateState(animManager, &(self->m_mainAnim), dt);

    GameObject_updateTransform(self);
}

static void ItemVM_onFixedUpdate(void* selfPtr, float timeStep)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Item* self = (Item*)selfPtr;

    GameContext* context = GameObject_getContext(self);
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    Body* body = GameObject_getBody(self);

    // Check collisions
    for (int i = 0; i < body->collisionCount; i++)
    {
        Collision* collision = &body->collisions[i];
        Body* otherBody = PhysicsEngine_getBody(physicsEngine, collision->other);

        GameObject* otherObj = (GameObject*)otherBody->userData;
        if (otherObj && otherObj->m_type == GAME_TYPE_PLAYER)
        {
            switch (self->m_type)
            {
            case ITEM_TYPE_CURE:
                Player_heal(otherObj, PLAYER_MAX_HP / 4.f);
                break;
            default:
                assert(false);
                break;
            }
            GameObject_setToDestroy(self);
        }
    }
}

static void ItemVM_onRender(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Item* self = (Item*)selfPtr;

    GameContext* context = GameObject_getContext(self);
    GraphicsSystem* graphicsSystem = GameContext_getGraphicsSystem(context);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);

    RenderAnchor anchor = { .value = Vec2_anchor_center };
    RenderSortingLayer sortingLayer = { 0 };
    RenderTexture texture = { 0 };

    GraphicsCmd cmd = { 0 };
    cmd.transform = GameObject_getTransform(self);
    cmd.sortingLayer = &sortingLayer;
    cmd.anchor = &anchor;
    cmd.texture = &texture;
    cmd.dimensions = &self->m_renderDim;

    sortingLayer.layer = GAME_LAYER_ITEM;

    RenderTexture_setFromAnim(&texture, animManager, &self->m_mainAnim);
    GraphicsSystem_addCommand(graphicsSystem, &cmd);
}

static void ItemVM_onDrawGizmos(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Item* self = (Item*)selfPtr;

    GameContext* context = GameObject_getContext(self);
    GizmosSystem* gizmosSystem = GameContext_getGizmosSystem(context);
    Body* body = GameObject_getBody(self);

    GizmosCircle circle = { .radius = body->colliderRadius };
    Transform transform = { .space = TRANSFORM_SPACE_WORLD, .position = body->transform.currPosition };
    GizmosCmd cmd = { 0 };
    cmd.transform = &transform;
    cmd.color = &g_colors.lime5;
    cmd.circle = &circle;
    GizmosSystem_addCommand(gizmosSystem, &cmd);
}

static void ItemVM_onTakeDamage(void* selfPtr, float damage, int playerId)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Item* self = (Item*)selfPtr;
}
