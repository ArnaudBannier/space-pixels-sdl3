/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/level/linear_bullet.h"
#include "game/core/game_context.h"

static void LinearBulletVM_onDestroy(void* selfPtr);
static void LinearBulletVM_onUpdate(void* selfPtr, float dt);
static void LinearBulletVM_onFixedUpdate(void* selfPtr, float timeStep);
static void LinearBulletVM_onRender(void* selfPtr);
static void LinearBulletVM_onDrawGizmos(void* selfPtr);

LinearBullet* LinearBullet_create(
    GameContext* context, LinearBulletType type,
    Vec2 position, float speed, Vec2 direction, int playerId)
{
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    AssetManager* assets = GameContext_getAssetManager(context);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);

    LinearBullet* self = (LinearBullet*)calloc(1, sizeof(LinearBullet));
    ASSERT_NEW(self);

    GameObject_init(self, context, GAME_TYPE_LINEAR_BULLET);

    RenderDim_setPixelsPerUnit(&self->m_renderDim, PIX_PER_UNIT);
    self->m_type = type;
    self->m_playerId = playerId;

    Vec2 velocity = Vec2_scale(direction, speed);
    float angle = Vec2_signedAngleDeg(Vec2_up, direction);

    Body* body = GameObject_getBody(self);
    body->velocity = velocity;
    body->angularVelocity = 0.f;
    body->categoryBits = GAME_CATEGORY_PLAYER_BULLET;
    body->maskBits = GAME_CATEGORY_ENEMY;
    FixedUpdateTransform_set(
        &(body->transform),
        TRANSFORM_SPACE_WORLD,
        position,
        angle
    );

    switch (type)
    {
    default:
    case LINEAR_BULLET_TYPE_PLAYER_DEFAULT:
        self->m_damage = DAMAGE_PLAYER_DEFAULT;
        body->categoryBits = GAME_CATEGORY_PLAYER_BULLET;
        body->maskBits = GAME_CATEGORY_ENEMY;
        body->colliderRadius = 0.05f;
        SpriteAnimState_init(&self->m_mainAnim, ANIM_BULLET_BASIC_2);
        break;
    case LINEAR_BULLET_TYPE_FRIGATE:
    case LINEAR_BULLET_TYPE_FIGHTER:
        // [TODO Collisions]
        // 1) Copier les catégorie/masque du tir du joueur
        // 2) Trouver les bonnes valeurs pour que les tirs des ennemis ne touchent pas les autres ennemis, mais touchent le joueur
        self->m_damage = DAMAGE_SMALL;
        body->categoryBits = 0;
        body->maskBits = 0;
        body->colliderRadius = 0.05f;
        SpriteAnimState_init(&self->m_mainAnim, ANIM_BULLET_BASIC_1);
        break;
    }

    GameObject* selfObj = (GameObject*)self;
    selfObj->m_onDestroy = LinearBulletVM_onDestroy;
    selfObj->m_onDrawGizmos = LinearBulletVM_onDrawGizmos;
    selfObj->m_onFixedUpdate = LinearBulletVM_onFixedUpdate;
    selfObj->m_onRender = LinearBulletVM_onRender;
    selfObj->m_onUpdate = LinearBulletVM_onUpdate;

    // Call update and fixed update to initialize
    LinearBulletVM_onFixedUpdate(self, 0.f);
    LinearBulletVM_onUpdate(self, 0.f);

    return self;
}

static void LinearBullet_emitParticles(LinearBullet* self)
{
    GameContext* context = GameObject_getContext(self);
    ParticleSystem* particleSystem = GameContext_getParticleSystem(context);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);
    const Transform* transform = GameObject_getTransform(self);
    int particleCount = 0;

    switch (self->m_type)
    {
    case LINEAR_BULLET_TYPE_PLAYER_DEFAULT:
        particleCount = 3;
        break;
    case LINEAR_BULLET_TYPE_FIGHTER:
    case LINEAR_BULLET_TYPE_FRIGATE:
        particleCount = 20;
        break;

    default:
        assert(false && "Unknown bullet type");
        particleCount = 0;
        break;
    }

    for (int i = 0; i < particleCount; i++)
    {
        Particle particle = { 0 };
        AnimId animId = ANIM_PARTICLE_0;
        Particle_init(&particle, animManager, animId);
        Particle_setLifetime(&particle, Float_randAB(0.5f, 1.0f));
        RenderDim_setPixelsPerUnit(&particle.dimensions, PIX_PER_UNIT);

        particle.velocity.x = Float_randAB(-5.0f, 5.0f);
        particle.velocity.y = Float_randAB(-5.0f, 5.0f);
        particle.transform = *transform;
        particle.damping = Vec2_scale(Vec2_one, 3.f);
        particle.sortingLayer.layer = GAME_LAYER_PARTICLE;

        particle.alphaValues.easing = EasingFct_In;
        particle.alphaValues.value0 = 1.f;
        particle.alphaValues.value1 = 0.f;

        ParticleSystem_emitParticle(particleSystem, &particle);
    }
}

static void LinearBulletVM_onDestroy(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    // Nothing to do
}

static void LinearBulletVM_onUpdate(void* selfPtr, float dt)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    LinearBullet* self = (LinearBullet*)selfPtr;

    GameContext* context = GameObject_getContext(self);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);

    SpriteAnimManager_updateState(animManager, &(self->m_mainAnim), dt);

    GameObject_updateTransform(self);
}

static void LinearBulletVM_onFixedUpdate(void* selfPtr, float timeStep)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    LinearBullet* self = (LinearBullet*)selfPtr;

    GameContext* context = GameObject_getContext(self);
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    Body* body = GameObject_getBody(self);


    // [TODO Collisions]
    // - Appliquez les dégâts aux autres objets touchés par le tir, en utilisant les informations de collision dans body->collisions.

    //// Check collisions
    //for (int i = 0; i < body->collisionCount; i++)
    //{
    //    Collision* collision = &body->collisions[i];
    //    Body* otherBody = PhysicsEngine_getBody(physicsEngine, collision->other);

    //    GameObject* otherObj = (GameObject*)otherBody->userData;
    //    GameObject_takeDamage(otherObj, self->m_damage, self->m_playerId);
    //    GameObject_setToDestroy(self);

    //    LinearBullet_emitParticles(self);
    //}

    // Check if out of bounds
    Vec2 position = body->transform.currPosition;
    if (position.x < -10.f || position.x > 10.f || position.y < -6.f || position.y > 6.f)
    {
        GameObject_setToDestroy(self);
    }
}

static void LinearBulletVM_onRender(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    LinearBullet* self = (LinearBullet*)selfPtr;

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

static void LinearBulletVM_onDrawGizmos(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    LinearBullet* self = (LinearBullet*)selfPtr;

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
