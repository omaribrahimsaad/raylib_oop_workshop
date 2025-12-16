#ifndef GAME_ENTITIES_H
#define GAME_ENTITIES_H

#include "shapes.h"
#include <format>
#define GRAVITY Vector2{0,200}
#define JUMP_COOLDOWN 3
#define PLAYER_JUMP_VEL  Vector2{0,-250}

class Entity
{
public:
    virtual void Update() = 0;

    std::shared_ptr<Shape> entity_shape_;
};

class Player : public Entity
{
public:
    Player(std::shared_ptr<Shape> player_shape)
    {
        entity_shape_ = player_shape;

        // set a constant gravity
        entity_shape_->acceleration_ = GRAVITY;
    }

    void Update()
    {
        entity_shape_->Update();
    }

};


/*
    This class is used to define the movements of 
    non-player-controlled entites.
*/

class Enemy : public Entity
{
public:

};

class Platform : public Entity
{
public:
    Platform(std::shared_ptr<Shape> platform_shape)
    {
        entity_shape_ = platform_shape;
    }

    void Update()
    {
        // update our shape's physics 
        entity_shape_->Update();
    }

};


#endif //GAME_ENTITIES_H