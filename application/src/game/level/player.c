/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/level/player.h"
#include "game/core/game_context.h"
#include "game/level/linear_bullet.h"
#include "game/level/homing_missile.h"

static void PlayerVM_onDestroy(void* selfPtr);
static void PlayerVM_onUpdate(void* selfPtr, float dt);
static void PlayerVM_onFixedUpdate(void* selfPtr, float timeStep);
static void PlayerVM_onRender(void* selfPtr);
static void PlayerVM_onDrawGizmos(void* selfPtr);
static void PlayerVM_onTakeDamage(void* selfPtr, float damage, int playerId);

static PlayerInput* Player_getInput(Player* self, GameContext* context);

Player* Player_create(GameContext* context, Vec2 position, int playerId)
{
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    AssetManager* assets = GameContext_getAssetManager(context);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);

    Player* self = (Player*)calloc(1, sizeof(Player));
    ASSERT_NEW(self);

    GameObject_init(self, context, GAME_TYPE_PLAYER);
    self->m_playerId = playerId;
    self->m_state = PLAYER_STATE_FLYING;
    self->m_bonusFlags = BONUS_ROCKETS;

    g_gameConfig.playerScores[playerId] = 0;
    g_gameConfig.playerHPs[playerId] = PLAYER_MAX_HP;

    self->m_maxHp = PLAYER_MAX_HP;

    // [TODO Player render]
    // - Attribuer la bonne animation principale au joueur (ANIM_PLAYER_SHIP)
    SpriteAnimState_init(&self->m_mainAnim, ANIM_PLAYER_SHIELD);
    SpriteAnimState_init(&self->m_engineAnim, ANIM_PLAYER_ENGINE);
    SpriteAnimState_init(&self->m_shieldAnim, ANIM_PLAYER_SHIELD);

    self->m_mainAnim.saveEvents = false;
    self->m_engineAnim.saveEvents = false;
    self->m_shieldAnim.saveEvents = false;

    // [TODO Player render]
    // - Affecter les dimensions du joueur en utilisant RenderDim_setPixelsPerUnit
    // pour que sa taille dans le monde soit proportionnelle à la taille de son sprite (voir PIX_PER_UNIT).

    RenderDim_setWorldDimensions(&self->m_renderDim, 5.f, 5.f);// A remplacer
    //RenderDim_setPixelsPerUnit(&self->m_renderDim, PIX_PER_UNIT);

    Body* body = GameObject_getBody(self);
    body->colliderRadius = 0.15f;
    body->categoryBits = GAME_CATEGORY_PLAYER;
    body->maskBits = GAME_CATEGORY_ENEMY_BULLET | GAME_CATEGORY_ITEM;

    FixedUpdateTransform_set(
        &(body->transform),
        TRANSFORM_SPACE_WORLD,
        position,
        -90.0f
    );

    GameObject* selfObj = (GameObject*)self;
    selfObj->m_onDestroy = PlayerVM_onDestroy;
    selfObj->m_onDrawGizmos = PlayerVM_onDrawGizmos;
    selfObj->m_onFixedUpdate = PlayerVM_onFixedUpdate;
    selfObj->m_onRender = PlayerVM_onRender;
    selfObj->m_onUpdate = PlayerVM_onUpdate;
    selfObj->m_onTakeDamage = PlayerVM_onTakeDamage;

    PlayerVM_onFixedUpdate(self, 0.f);
    PlayerVM_onUpdate(self, 0.f);

    g_gameConfig.level.activePlayerCount++;

    return self;
}

static PlayerInput* Player_getInput(Player* self, GameContext* context)
{
    Input* input = GameContext_getInput(context);
    return &(input->players[self->m_playerId]);
}

void Player_addBonus(void* selfPtr, int flags)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Player* self = (Player*)selfPtr;
    assert(((GameObject*)self)->m_type == GAME_TYPE_PLAYER && "self must be a Player");

    //if (flags & BONUS_AUTO_CANONS)
    //{
    //    self->m_bonusFlags |= BONUS_AUTO_CANONS;
    //}
}

void Player_addPoints(void* selfPtr, float points)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Player* self = (Player*)selfPtr;
    assert(((GameObject*)self)->m_type == GAME_TYPE_PLAYER && "self must be a Player");
    g_gameConfig.playerScores[self->m_playerId] += points;
}

void Player_heal(void* selfPtr, float hp)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Player* self = (Player*)selfPtr;
    assert(((GameObject*)self)->m_type == GAME_TYPE_PLAYER && "self must be a Player");
    g_gameConfig.playerHPs[self->m_playerId] += hp;
}

static void PlayerVM_onDestroy(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    // Nothing to do
}

static void Player_updateBullet(Player* self, PlayerInput* playerInput, float dt)
{
    GameContext* context = GameObject_getContext(self);
    Transform* transform = GameObject_getTransform(self);

    // [TODO Player shoot] V1
    // - Implémenter un tir simple qui se déclenche lorsque le joueur appuie sur le bouton de tir (shootPressed).
    // - Comparez shootPressed avec shootDown.

    //if (playerInput->shootPressed)
    //{
    //    Vec2 position = transform->position;
    //    const int playerId = self->m_playerId;
    //    const LinearBulletType bulletType = LINEAR_BULLET_TYPE_PLAYER_DEFAULT;
    //    LinearBullet* bullet = LinearBullet_create(context, bulletType, position, 8.f, Vec2_right, playerId);
    //    GameContext_playSpatialSFX(context, AUDIO_SFX_PLAYER_BASIC, transform);
    //}

    // [TODO Player shoot] V2
    // - Implémenter un système de cadence de tir en utilisant l'accumulateur m_accuBullet.
    
    //self->m_accuBullet -= dt;
    //if ((playerInput->triggerR > 0.5f || playerInput->shootDown) && self->m_accuBullet < 0.f)
    //{
    //    Vec2 position = transform->position;
    //    const int playerId = self->m_playerId;
    //    const LinearBulletType bulletType = LINEAR_BULLET_TYPE_PLAYER_DEFAULT;
    //    LinearBullet* bullet = LinearBullet_create(context, bulletType, position, 8.f, Vec2_right, playerId);
    //    self->m_accuBullet = PLAYER_BULLET_TIME;

    //    GameContext_playSpatialSFX(context, AUDIO_SFX_PLAYER_BASIC, transform);
    //}
}

static void PlayerVM_onUpdate(void* selfPtr, float dt)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Player* self = (Player*)selfPtr;

    if (self->m_state == PLAYER_STATE_DEAD) return;
    if (g_gameConfig.level.state != GAME_STATE_PLAYING) return;

    GameContext* context = GameObject_getContext(self);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);
    PlayerInput* playerInput = Player_getInput(self, context);

    // [TODO Player render]
    // - Mettre à jour les animations du joueur en utilisant SpriteAnimManager_updateState.
    //SpriteAnimManager_updateState(animManager, &(self->m_mainAnim), dt);
    //SpriteAnimManager_updateState(animManager, &(self->m_engineAnim), dt);
    //SpriteAnimManager_updateState(animManager, &(self->m_shieldAnim), dt);

    // [TODO Player movement]
    // - Mettez à jour la direction et la vitesse du joueur en fonction de l'input.
    
    //self->m_direction = playerInput->axis;
    //self->m_speed = Float_lerp(4.f, 6.f, playerInput->triggerL);

    GameObject_updateTransform(self);

    Player_updateBullet(self, playerInput, dt);
}

static void PlayerVM_onFixedUpdate(void* selfPtr, float timeStep)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Player* self = (Player*)selfPtr;

    if (self->m_state == PLAYER_STATE_DEAD) return;

    GameContext* context = GameObject_getContext(self);
    PhysicsEngine* physicsEngine = GameContext_getPhysicsEngine(context);
    Body* body = GameObject_getBody(self);
    PlayerInput* playerInput = Player_getInput(self, context);
    //Camera* camera = GameContext_getCamera(context);
    //AABB worldView = Camera_getWorldView(camera);


    // [TODO Player movement]
    // - Appliquez la direction et la vitesse du joueur à son corps pour le faire bouger.

    //body->velocity = Vec2_scale(self->m_direction, self->m_speed);

    self->m_accuShield -= timeStep;
}

static void PlayerVM_onTakeDamage(void* selfPtr, float damage, int playerId)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Player* self = (Player*)selfPtr;

    GameContext* context = GameObject_getContext(self);

    if (self->m_accuShield > 0.f) return;
    if (self->m_state != PLAYER_STATE_FLYING) return;

    float playerHP = g_gameConfig.playerHPs[self->m_playerId];
    playerHP -= damage;
    if (playerHP <= 0.f)
    {
        self->m_state = PLAYER_STATE_DEAD;

        Body* body = GameObject_getBody(self);
        body->categoryBits = 0;
        body->maskBits = 0;

        g_gameConfig.level.activePlayerCount--;
    }

    g_gameConfig.playerHPs[self->m_playerId] = playerHP;
}

static void PlayerVM_onRender(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Player* self = (Player*)selfPtr;

    if (self->m_state == PLAYER_STATE_DEAD) return;

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
    cmd.dimensions = &self->m_renderDim;
    cmd.texture = &texture;

    sortingLayer.layer = GAME_LAYER_PLAYER;

    // [TODO Player render]
    // - Dessiner le joueur en utilisant les animations principales, moteur et bouclier.

    //RenderTexture_setFromAnim(&texture, animManager, &self->m_mainAnim);
    //GraphicsSystem_addCommand(graphicsSystem, &cmd);

    //RenderTexture_setFromAnim(&texture, animManager, &self->m_engineAnim);
    //GraphicsSystem_addCommand(graphicsSystem, &cmd);

    //if (self->m_accuShield > 0.f)
    //{
    //    RenderTexture_setFromAnim(&texture, animManager, &self->m_shieldAnim);
    //    GraphicsSystem_addCommand(graphicsSystem, &cmd);
    //}
}

static void PlayerVM_onDrawGizmos(void* selfPtr)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    Player* self = (Player*)selfPtr;

    GameContext* context = GameObject_getContext(self);
    GizmosSystem* gizmosSystem = GameContext_getGizmosSystem(context);
    Body* body = GameObject_getBody(self);

    GizmosCircle circle = { .radius = body->colliderRadius };
    Transform transform = { .space = TRANSFORM_SPACE_WORLD, .position = body->transform.currPosition };
    GizmosCmd cmd = { 0 };
    cmd.transform = &transform;
    cmd.color = &g_colors.red5;
    cmd.circle = &circle;
    GizmosSystem_addCommand(gizmosSystem, &cmd);
}
