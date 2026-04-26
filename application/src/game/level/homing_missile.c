/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/level/homing_missile.h"
#include "game/core/game_context.h"

static void HomingMissileVM_onDestroy(void* selfPtr);
static void HomingMissileVM_onUpdate(void* selfPtr, float dt);
static void HomingMissileVM_onFixedUpdate(void* selfPtr, float timeStep);
static void HomingMissileVM_onRender(void* selfPtr);
static void HomingMissileVM_onDrawGizmos(void* selfPtr);

HomingMissile* HomingMissile_create(
    GameContext* context, HomingMissileType type,
    Vec2 position, float speed, Vec2 direction, int playerId)
{
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    AssetManager* assets = GameContext_getAssetManager(context);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);

    HomingMissile* self = (HomingMissile*)calloc(1, sizeof(HomingMissile));
    ASSERT_NEW(self);

    GameObject_init(self, context, GAME_TYPE_HOMING_MISSILE);
    self->m_type = type;
    self->m_playerId = playerId;

    Vec2 velocity = Vec2_scale(direction, speed);
    float angle = Vec2_signedAngleDeg(Vec2_up, direction);

    RenderDim_setPixelsPerUnit(&self->m_renderDim, PIX_PER_UNIT);
    self->m_targetBodyId = BodyId_null;
    self->m_targetMask = 0;
    self->m_angle = angle;
    self->m_speed = speed;

    Body* body = GameObject_getBody(self);
    body->velocity = velocity;
    body->angularVelocity = 0.f;
    FixedUpdateTransform_set(
        &(body->transform),
        TRANSFORM_SPACE_WORLD,
        position,
        angle
    );

    switch (type)
    {
    default:
    case HOMING_MISSILE_TYPE_PLAYER_ROCKET:
        self->m_damage = DAMAGE_MEDIUM;
        self->m_targetMask = GAME_CATEGORY_ENEMY;
        self->m_homingTime = 0.2f;
        self->m_searchRadius = 3.f;
        body->categoryBits = GAME_CATEGORY_PLAYER_BULLET;
        body->maskBits = GAME_CATEGORY_ENEMY;
        body->colliderRadius = 0.05f;
        SpriteAnimState_init(&self->m_mainAnim, ANIM_BULLET_ROCKET);
        break;
    }

    GameObject* selfObj = (GameObject*)self;
    selfObj->m_onDestroy = HomingMissileVM_onDestroy;
    selfObj->m_onDrawGizmos = HomingMissileVM_onDrawGizmos;
    selfObj->m_onFixedUpdate = HomingMissileVM_onFixedUpdate;
    selfObj->m_onRender = HomingMissileVM_onRender;
    selfObj->m_onUpdate = HomingMissileVM_onUpdate;

    // Call update and fixed update to initialize
    HomingMissileVM_onFixedUpdate(self, 0.f);
    HomingMissileVM_onUpdate(self, 0.f);

    return self;
}

static void HomingMissileVM_onDestroy(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    // Nothing to do
}

static void HomingMissileVM_onUpdate(void* selfPtr, float dt)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    HomingMissile* self = (HomingMissile*)selfPtr;

    GameContext* context = GameObject_getContext(self);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);

    SpriteAnimManager_updateState(animManager, &(self->m_mainAnim), dt);

    GameObject_updateTransform(self);
}

static void HomingMissileVM_onFixedUpdate(void* selfPtr, float timeStep)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    HomingMissile* self = (HomingMissile*)selfPtr;

    GameContext* context = GameObject_getContext(self);
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    Body* body = GameObject_getBody(self);
    Body* targetBody = PhysicsEngine_getBody(physicsEngine, self->m_targetBodyId);

    if (targetBody == NULL)
    {
        // Search for a new target
        Vec2 direction = Vec2_normalize(body->velocity);
        Vec2 center = body->transform.currPosition;
        center = Vec2_add(center, Vec2_scale(direction, self->m_searchRadius));
        BodyId targetBodyIds[16] = { 0 };
        const int targetCount = PhysicsEngine_queryCircle(
            physicsEngine, center, self->m_searchRadius, self->m_targetMask,
            targetBodyIds, 16
        );

        if (targetCount > 0)
        {
            self->m_targetBodyId = targetBodyIds[0];
        }
        targetBody = PhysicsEngine_getBody(physicsEngine, self->m_targetBodyId);
    }

    if (targetBody != NULL)
    {
        Vec2 toTarget = Vec2_sub(
            targetBody->transform.currPosition,
            body->transform.currPosition
        );
        float angleToTarget = Vec2_signedAngleDeg(Vec2_up, toTarget);

        float speed = self->m_speed;
        self->m_angle = Float_smoothDamp(
            self->m_angle, angleToTarget, &self->m_angularVelocity,
            self->m_homingTime, 180.f, timeStep
        );

        float angleRad = (self->m_angle + 90.f) * (float)M_PI / 180.f;
        body->velocity = Vec2_set(cosf(angleRad) * speed, sinf(angleRad) * speed);
        body->transform.currAngle = Vec2_signedAngleDeg(Vec2_up, body->velocity);
    }

    // Check collisions
    for (int i = 0; i < body->collisionCount; i++)
    {
        Collision* collision = &body->collisions[i];
        Body* otherBody = PhysicsEngine_getBody(physicsEngine, collision->other);

        GameObject* otherObj = (GameObject*)otherBody->userData;
        GameObject_takeDamage(otherObj, self->m_damage, self->m_playerId);
        GameObject_setToDestroy(self);
    }

    // Check if out of bounds
    Vec2 position = body->transform.currPosition;
    if (position.x < -10.f || position.x > 10.f || position.y < -6.f || position.y > 6.f)
    {
        GameObject_setToDestroy(self);
    }
}

static void HomingMissileVM_onRender(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    HomingMissile* self = (HomingMissile*)selfPtr;

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

    sortingLayer.layer = GAME_LAYER_BULLET;

    RenderTexture_setFromAnim(&texture, animManager, &self->m_mainAnim);
    GraphicsSystem_addCommand(graphicsSystem, &cmd);
}

static void HomingMissileVM_onDrawGizmos(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    HomingMissile* self = (HomingMissile*)selfPtr;

    GameContext* context = GameObject_getContext(self);
    GizmosSystem* gizmosSystem = GameContext_getGizmosSystem(context);
    Body* body = GameObject_getBody(self);

    GizmosCircle circle = { .radius = body->colliderRadius };
    Transform transform = { .space = TRANSFORM_SPACE_WORLD, .position = body->transform.currPosition };
    GizmosCmd cmd = { 0 };
    cmd.transform = &transform;
    cmd.color = &g_colors.cyan5;
    cmd.circle = &circle;
    GizmosSystem_addCommand(gizmosSystem, &cmd);
}
