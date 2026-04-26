/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/level/background_layer.h"
#include "game/core/game_context.h"

static void BackgroundLayerVM_onDestroy(void* selfPtr);
static void BackgroundLayerVM_onUpdate(void* selfPtr, float dt);
static void BackgroundLayerVM_onFixedUpdate(void* selfPtr, float timeStep);
static void BackgroundLayerVM_onRender(void* selfPtr);
static void BackgroundLayerVM_onDrawGizmos(void* selfPtr);
static void BackgroundLayerVM_onTakeDamage(void* selfPtr, float damage, int playerId);

BackgroundLayer* BackgroundLayer_create(
    GameContext* context,
    Vec2 worldDimensions, Vec2 parallaxSpeed,
    SpriteGroup* spriteGroup, int spriteIndex, int orderInLayer)
{
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    AssetManager* assets = GameContext_getAssetManager(context);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);

    BackgroundLayer* self = (BackgroundLayer*)calloc(1, sizeof(BackgroundLayer));
    ASSERT_NEW(self);

    GameObject_init(self, context, GAME_TYPE_UNDEFINED);

    // Background layers are purely visual and should not have a physics body,
    // so we destroy the default body created by GameObject_init.
    GameObject* selfObj = (GameObject*)self;
    PhysicsEngine_destroyBody(physicsEngine, selfObj->m_bodyId);

    selfObj->m_onDestroy = BackgroundLayerVM_onDestroy;
    selfObj->m_onDrawGizmos = BackgroundLayerVM_onDrawGizmos;
    selfObj->m_onFixedUpdate = BackgroundLayerVM_onFixedUpdate;
    selfObj->m_onRender = BackgroundLayerVM_onRender;
    selfObj->m_onUpdate = BackgroundLayerVM_onUpdate;
    selfObj->m_onTakeDamage = BackgroundLayerVM_onTakeDamage;

    RenderDim_setWorldDimensions(&self->m_renderDim, worldDimensions.x, worldDimensions.y);
    self->m_parallaxSpeed = parallaxSpeed;
    self->m_worldDimensions = worldDimensions;
    self->m_spriteGroup = spriteGroup;
    self->m_spriteIndex = spriteIndex;
    self->m_sortingLayer.layer = GAME_LAYER_BACKGROUND;
    self->m_sortingLayer.orderInLayer = orderInLayer;

    BackgroundLayerVM_onFixedUpdate(self, 0.f);
    BackgroundLayerVM_onUpdate(self, 0.f);

    return self;
}

static void BackgroundLayerVM_onDestroy(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    BackgroundLayer* self = (BackgroundLayer*)selfPtr;
}

static void BackgroundLayerVM_onUpdate(void* selfPtr, float dt)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    BackgroundLayer* self = (BackgroundLayer*)selfPtr;

    Transform* transform = GameObject_getTransform(self);
    transform->position.x += self->m_parallaxSpeed.x * dt;
    transform->position.y += self->m_parallaxSpeed.y * dt;

    GameContext* context = GameObject_getContext(self);
    Camera* camera = GameContext_getCamera(context);
    AABB worldView = Camera_getWorldView(camera);
}

static void BackgroundLayerVM_onFixedUpdate(void* selfPtr, float timeStep)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    BackgroundLayer* self = (BackgroundLayer*)selfPtr;
}

static void BackgroundLayerVM_onRender(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    BackgroundLayer* self = (BackgroundLayer*)selfPtr;

    GameContext* context = GameObject_getContext(self);
    GraphicsSystem* graphicsSystem = GameContext_getGraphicsSystem(context);
    Camera* camera = GameContext_getCamera(context);
    AABB worldView = Camera_getWorldView(camera);

    Transform* transform = GameObject_getTransform(self);
    Vec2 layerWest = { 0 };
    layerWest.x = transform->position.x - worldView.lower.x;
    layerWest.y = transform->position.y - 0.5f * (worldView.lower.y + worldView.upper.y);

    layerWest.x = fmodf(layerWest.x, self->m_renderDim.width);
    layerWest.y = fmodf(layerWest.y, self->m_renderDim.height);
    layerWest.x -= layerWest.x > 0.f ? self->m_renderDim.width : 0.f;
    layerWest.y -= layerWest.y > 0.f ? self->m_renderDim.height : 0.f;

    layerWest = Vec2_add(layerWest, worldView.lower);

    Transform backTransform = { 0 };
    backTransform.position = layerWest;
    backTransform.angle = 0.f;
    backTransform.space = TRANSFORM_SPACE_WORLD;

    RenderAnchor anchor = { .value = Vec2_anchor_west };
    RenderTexture texture = { 0 };

    GraphicsCmd cmd = { 0 };
    cmd.transform = &backTransform;
    cmd.sortingLayer = &self->m_sortingLayer;
    cmd.anchor = &anchor;
    cmd.dimensions = &self->m_renderDim;
    cmd.texture = &texture;

    while (backTransform.position.x < worldView.upper.x)
    {
        RenderTexture_setFromGroup(&texture, self->m_spriteGroup, self->m_spriteIndex);
        GraphicsSystem_addCommand(graphicsSystem, &cmd);

        backTransform.position.x += self->m_renderDim.width;
    }
}

static void BackgroundLayerVM_onDrawGizmos(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    BackgroundLayer* self = (BackgroundLayer*)selfPtr;
}

static void BackgroundLayerVM_onTakeDamage(void* selfPtr, float damage, int playerId)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    BackgroundLayer* self = (BackgroundLayer*)selfPtr;
}
