/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "game/level/enemy_data.h"
#include "game/core/game_context.h"
#include "game/game_config.h"

void Enemy_init(void* selfPtr, EnemyData* data)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    GameObject* self = (GameObject*)selfPtr;
    GameContext* context = GameObject_getContext(self);

    // Register the enemy in the level manager
    g_gameConfig.level.enemyCount++;

    data->state = ENEMY_STATE_FIRING;
    data->hasShield = false;

    SpriteAnimState_init(&data->m_mainAnim, data->idleAnimId);
    SpriteAnimState_init(&data->m_shieldAnim, data->shieldAnimId);
    data->m_shieldAnim.saveEvents = false;

    RenderDim_setPixelsPerUnit(&data->m_renderDim, PIX_PER_UNIT);

    Body* body = GameObject_getBody(self);
    body->colliderRadius = 1.0f;
    body->categoryBits = GAME_CATEGORY_ENEMY;
    body->maskBits = GAME_CATEGORY_PLAYER_BULLET;
    FixedUpdateTransform_set(
        &(body->transform),
        TRANSFORM_SPACE_WORLD,
        Vec2_set(+15.f, 0.f),
        +90.0f
    );
}

void Enemy_onUpdate(void* selfPtr, EnemyData* data, float dt)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    GameObject* self = (GameObject*)selfPtr;

    GameContext* context = GameObject_getContext(self);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);
    SpriteAnimState* mainAnim = &data->m_mainAnim;

    GameObject_updateTransform(self);

    switch (data->state)
    {
    case ENEMY_STATE_FIRING:
        if (mainAnim->id != data->firingAnimId)
        {
            mainAnim->id = data->firingAnimId;
            SpriteAnimState_play(mainAnim);
        }
        break;

        // [TODO Collisions]
        // - Changer l'animation de l'ennemi lorsque son état change pour ENEMY_STATE_DYING.

        //case ENEMY_STATE_DYING:
        //    if (mainAnim->id != data->dyingAnimId)
        //    {
        //        mainAnim->id = data->dyingAnimId;
        //        SpriteAnimState_play(mainAnim);
        //    }
        //    break;
    default:
        break;
    }
    // [TODO enemy]
    // - Mettre à jour l'animation principale de l'ennemi en utilisant SpriteAnimManager_updateState.
    
    //SpriteAnimManager_updateState(animManager, mainAnim, dt);

    if (data->hasShield)
    {
        if (SpriteAnimState_isPlaying(&data->m_shieldAnim) == false)
        {
            data->m_shieldAnim.id = data->shieldAnimId;
            SpriteAnimState_play(&data->m_shieldAnim);
        }
        SpriteAnimManager_updateState(animManager, &(data->m_shieldAnim), dt);
    }

    // [TODO Collisions]
    // - Détruisez l'ennemi lorsque son animation de mort est terminée.
    // Pour cela, vous pouvez écouter l'événement ANIM_EVENT_ANIMATION_END.

    //for (int i = 0; i < mainAnim->eventCount; i++)
    //{
    //    SpriteAnimEvent* event = &(mainAnim->events[i]);
    //    if (event->id == data->dyingAnimId &&
    //        event->type == ANIM_EVENT_ANIMATION_END)
    //    {
    //        GameObject_setToDestroy(self);
    //        break;
    //    }
    //}
}

void Enemy_onFixedUpdate(void* selfPtr, EnemyData* data, float timeStep)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    GameObject* self = (GameObject*)selfPtr;

    Vec2 position = data->position;
    Body* body = GameObject_getBody(self);
    body->transform.currPosition = position;
}

void Enemy_onTakeDamage(void* selfPtr, EnemyData* data, float damage, int playerId)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    GameObject* self = (GameObject*)selfPtr;

    if (data->state == ENEMY_STATE_DYING) return;
    if (data->hasShield) return;

    // [TODO Collisions]
    // - Diminuez les points de vie de l'ennemi en fonction des dégâts reçus.
    
    //data->hp -= damage;

    if (data->hp <= 0.f)
    {
        data->hp = 0.f;

        // [TODO Collisions]
        // - Changer l'état de l'ennemi pour ENEMY_STATE_DYING.
        
        //data->state = ENEMY_STATE_DYING;

        Body* body = GameObject_getBody(self);
        body->maskBits = 0; // Disable collisions

        GameContext* context = GameObject_getContext(self);
        Transform* transform = GameObject_getTransform(self);
        GameContext_playSpatialSFX(context, AUDIO_SFX_ENEMY_EXPLOSION, transform);

        // [TODO Collisions]
        // - Augmenter le score du joueur qui a tué l'ennemi
        
        //g_gameConfig.playerScores[playerId] += (int)data->points;

        if (data->itemType != ITEM_TYPE_NONE)
        {
            GameContext* context = GameObject_getContext(self);
            Transform* transform = GameObject_getTransform(self);
            (void)Item_create(context, data->itemType, transform->position);
        }
    }
}

void Enemy_onRender(void* selfPtr, EnemyData* data)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    GameObject* self = (GameObject*)selfPtr;

    GameContext* context = GameObject_getContext(self);
    GraphicsSystem* graphicsSystem = GameContext_getGraphicsSystem(context);
    SpriteAnimManager* animManager = GameContext_getAnimManager(context);

    RenderAnchor anchor = { .value = Vec2_anchor_center };
    RenderSortingLayer sortingLayer = { 0 };
    RenderTexture texture = { 0 };

    // [TODO enemy]
    // - Dessiner l'ennemi en utilisant les informations de data->m_mainAnim.

    //GraphicsCmd cmd = { 0 };
    //cmd.transform = GameObject_getTransform(self);
    //cmd.sortingLayer = &sortingLayer;
    //cmd.anchor = &anchor;
    //cmd.texture = &texture;
    //cmd.dimensions = &data->m_renderDim;

    //sortingLayer.layer = GAME_LAYER_ENEMY;

    //RenderTexture_setFromAnim(&texture, animManager, &data->m_mainAnim);
    //GraphicsSystem_addCommand(graphicsSystem, &cmd);

    // [Bonus]
    // - Dessiner le bouclier de l'ennemi si il en a un.
}

void Enemy_onDrawGizmos(void* selfPtr, EnemyData* data, SDL_Color* color)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    GameObject* self = (GameObject*)selfPtr;

    GameContext* context = GameObject_getContext(self);
    GizmosSystem* gizmosSystem = GameContext_getGizmosSystem(context);
    Body* body = GameObject_getBody(self);

    // [TODO enemy guizmos]
    // - Dessiner un cercle de la couleur donnée autour de l'ennemi pour visualiser son collider.

    //GizmosCircle circle = { .radius = body->colliderRadius };
    //Transform transform = { .space = TRANSFORM_SPACE_WORLD, .position = body->transform.currPosition };
    //GizmosCmd cmd = { 0 };
    //cmd.transform = &transform;
    //cmd.color = color;
    //cmd.circle = &circle;
    //GizmosSystem_addCommand(gizmosSystem, &cmd);
}

void Enemy_onDestroy(void* selfPtr, EnemyData* data)
{
    assert(GameObject_isValid(selfPtr) && "self must be a valid GameObject");
    GameObject* self = (GameObject*)selfPtr;

    GameContext* context = GameObject_getContext(self);

    // Unregister the enemy from the level
    g_gameConfig.level.enemyCount--;
}
