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
        entity_shape_->enable_screen_edge_collision_ = false;
    }

    void Update()
    {
        entity_shape_->Update();
    }

    void Jump()
    {
        auto elapsed_time_since_jump = GetTime() - last_jump_time_;    

        if(elapsed_time_since_jump > JUMP_COOLDOWN || bypass_cooldown)
        {
            // jump if enough time elapsed since the last time we jumped 
            entity_shape_->velocity_ = PLAYER_JUMP_VEL;

            last_jump_time_ = GetTime();

            bypass_cooldown = false;
        }
    }

    bool bypass_cooldown = false;
private:
    float last_jump_time_ = 0;

    
};


/*
    This class is used to define the movements of 
    non-player-controlled entites.
*/
struct PathPlan
{
    // now just support horizonal movement for agents
    float horizontal_distance_,movement_speed_;
};

class Enemy : public Entity
{
public:
    Enemy(std::shared_ptr<Shape> enemy_shape,PathPlan path_plan)
    {
        entity_shape_ = enemy_shape;
        enemy_path_plan_ = path_plan;
        
        // get the starting position from the initial enemy shape.
        starting_position_ = enemy_shape->position_;
        point_left_ = point_right_ = starting_position_;
        point_left_.x -= path_plan.horizontal_distance_;
        point_right_.x += path_plan.horizontal_distance_;
        
        // set initial enemy velocity:
        entity_shape_->velocity_ = {path_plan.movement_speed_,0};

        // set a constant gravity
        entity_shape_->acceleration_ = GRAVITY;

        // disable screen collsion for offscreen objects
        entity_shape_->enable_screen_edge_collision_ = false;
    }

    void Update()
    {
        // Add handling for enemy movements and other behaviours here
        Vector2 current_enemy_velocity;
        // std::cout << std::format("current position: {},{} point: {},{} to point: {},{}\n",
            // entity_shape_->position_.x,entity_shape_->position_.y,
            // point_left_.x,point_left_.y,point_right_.x,point_right_.y);

        if(entity_shape_->position_.x < point_left_.x)
        {
            // move right
            entity_shape_->velocity_.x = std::abs(entity_shape_->velocity_.x);

        }
        else if(entity_shape_->position_.x  >  point_right_.x)
        {
            // move left
            entity_shape_->velocity_.x = -std::abs(entity_shape_->velocity_.x);

        }
        // std::cout << std::format("enemy velocity: {},{}\n",entity_shape_->velocity_.x,entity_shape_->velocity_.y);
        // std::cout << std::format("point: {},{} to point: {},{}\n",point_left_.x,point_left_.y,point_right_.x,point_right_.y);
        // std::cout << std::format("current position: {},{}\n",entity_shape_->position_.x,entity_shape_->position_.y);

        // update our shapes physics
        entity_shape_->Update();
        
    }

private:
    PathPlan enemy_path_plan_;
    Vector2 starting_position_;
    Vector2 point_left_,point_right_; // the points the enemy will go back and forth 

};

class Platform : public Entity
{
public:
    Platform(std::shared_ptr<Shape> platform_shape)
    {
        entity_shape_ = platform_shape;
        entity_shape_->enable_screen_edge_collision_ = false;
    }

    void Update()
    {
        // update our shape's physics 
        entity_shape_->Update();
    }

};

// defines the winning state of the game.
class WinPlatform : public Platform
{
public:
    WinPlatform(std::shared_ptr<Shape> platform_shape)
        :Platform(platform_shape)
    {}
};

#endif //GAME_ENTITIES_H