#ifndef GAME_H
#define GAME_H
#include "raylib.h"
#include "raymath.h"
#include <memory>
#include "game_entities.h"
#include <vector>
#include <map>

class Level
{
public:
    Level(std::shared_ptr<Camera2D> level_camera, Vector2 player_spawn_point)
    {
        player_spawn_point_ = player_spawn_point;
        level_camera_ = level_camera;
    }
    
    void AddPlatforms(std::vector<std::shared_ptr<Platform>> platforms)
    {
        for(auto platform : platforms)
            entities_.push_back(platform);
        
    }

    void AddEnemies(std::vector<std::shared_ptr<Enemy>> enemies)
    {
        for(auto enemy : enemies)
            entities_.push_back(enemy);

    }


    std::vector<std::shared_ptr<Entity>> GetLevelEntities()
    {
        return entities_;
    }

    Vector2 GetPlayerSpawnPoint()
    {
        return player_spawn_point_;
    }


    std::shared_ptr<Camera2D> GetLevelCamera()
    {
        return level_camera_;
    }

private:
    std::vector<std::shared_ptr<Entity>> entities_;
    Vector2 player_spawn_point_;
    std::shared_ptr<Camera2D> level_camera_;
};


// utilities
struct Text
{
    const char* text_;
    int font_size_;
    Color text_color_;
    
    int GetTextWidth(){return MeasureText(text_,font_size_);}
    void DrawTextOnScreen(float x,float y)
    {
        DrawText(
            text_,x - GetTextWidth()/2.f,
            y-font_size_/2.f,
            font_size_,
            text_color_);

    }
};

// game state can help us to keep track of the state of the game
enum GameState
{
    kDead = 0, // the player died so can handle on restart
    kPlaying = 1, // the player is playing so we just run the current level continously
    kWin = 2, // the player won if there is a next level lined up then we run it else we show the win screen
};

class Game
{
public:
    Game(std::shared_ptr<Player> player)
    {
        player_ = player;
    }


    void SetPlayerShape(std::shared_ptr<Shape> new_shape)
    {
        // maintain the existing player state only update the shape info
        new_shape->position_ = player_->entity_shape_->position_;
        new_shape->acceleration_ = player_->entity_shape_->acceleration_;
        new_shape->velocity_ = player_->entity_shape_->velocity_;

        player_->entity_shape_ = new_shape;


    }
    
    void SetLevel(std::shared_ptr<Level> level)
    {
        current_level_ = level;
    }

    void StartGame()
    {
        // initialize the game using the level info

        // set the camera and player position according to the spawn point
        auto player_spawn_point = current_level_->GetPlayerSpawnPoint();
        
        current_level_->GetLevelCamera()->target = player_spawn_point;
        player_->entity_shape_->position_ = player_spawn_point;
        current_game_state_ = kPlaying;
    }

    /*
        Where the core game logic sits and to be updated on every frame.
    */
    void Update()
    {
        // this is the state machine running our game
        switch(current_game_state_)
        {
            case kPlaying:
            {
                // Input handling flow:
                // first the movement control:
                // initialize the new frame state for all entities
                auto all_game_entities = current_level_->GetLevelEntities();
                all_game_entities.push_back(player_);

                for(auto entity: all_game_entities)
                {
                    entity->entity_shape_->OnNewFrame();
                }


                // Collision checking for entities
                all_game_entities.push_back(player_);

                for(auto entity : all_game_entities)
                {
                    // add special handling for collsions based on the entity type:
                    auto player = std::dynamic_pointer_cast<Player>(entity);
                    auto platform = std::dynamic_pointer_cast<Platform>(entity);
                    auto win_platform = std::dynamic_pointer_cast<WinPlatform>(entity);
                    auto enemy = std::dynamic_pointer_cast<Enemy>(entity);
                    
                    for(auto other_entity : all_game_entities)
                    {
                        auto other_player = std::dynamic_pointer_cast<Player>(other_entity);
                        auto other_platform = std::dynamic_pointer_cast<Platform>(other_entity);
                        auto other_win_platform = std::dynamic_pointer_cast<WinPlatform>(other_entity);
                        auto other_enemy = std::dynamic_pointer_cast<Enemy>(other_entity);

                        
                        bool player_with_platform = ((player && other_platform ) || (other_player && platform));
                        bool player_with_win_platform = ((player && other_win_platform ) || (other_player && win_platform));
                        bool enemy_with_platform = ((enemy && other_platform ) || (other_enemy && platform));
                        bool enemy_with_player = ((enemy && other_player ) || (other_enemy && player));
                        
                        // handle the collsion between the player and the ground
                        if(player_with_platform)
                        {
                            if(player && other_platform )
                            {
                                auto previous_position = other_platform->entity_shape_->position_;
                                auto previous_player_velocity = player->entity_shape_->velocity_;
                                
                                bool collides = player->entity_shape_->Collides(other_platform->entity_shape_);
                                if(collides)
                                {
                                    other_platform->entity_shape_->velocity_ = {0,0};
                                    other_platform->entity_shape_->position_ = previous_position;
                                    player->entity_shape_->velocity_.y = 0;
                                    player->entity_shape_->velocity_.x = previous_player_velocity.x;
                                    player->bypass_cooldown = true;
                                }

                            }

                            if(other_player && platform)
                            {
                                auto previous_position = platform->entity_shape_->position_;
                                auto previous_player_velocity = other_player->entity_shape_->velocity_;
                                bool collides = platform->entity_shape_->Collides(other_player->entity_shape_);
                                if(collides)
                                {
                                    platform->entity_shape_->velocity_ = {0,0};
                                    platform->entity_shape_->position_ = previous_position;
                                    other_player->entity_shape_->velocity_.y = 0;
                                    other_player->entity_shape_->velocity_.x = previous_player_velocity.x;
                                    other_player->bypass_cooldown = true;
                                }
                            }
                                
                        }

                        // handle the collision between the player and the enemy
                        else if(enemy_with_platform )
                        {
                            if(enemy && other_platform )
                            {
                                auto previous_position = other_platform->entity_shape_->position_;
                                auto previous_enemy_velocity = enemy->entity_shape_->velocity_;
                                bool collides = enemy->entity_shape_->Collides(other_platform->entity_shape_);
                                if(collides)
                                {
                                    other_platform->entity_shape_->velocity_ = {0,0};
                                    other_platform->entity_shape_->position_ = previous_position;
                                    enemy->entity_shape_->velocity_.y = 0;
                                    enemy->entity_shape_->velocity_.x = previous_enemy_velocity.x;
                                }
                            }

                            if(other_enemy && platform)
                            {
                                auto previous_position = platform->entity_shape_->position_;
                                auto previous_enemy_velocity = other_enemy->entity_shape_->velocity_;
                                bool collides = platform->entity_shape_->Collides(other_enemy->entity_shape_);
                                if(collides)
                                {
                                    platform->entity_shape_->velocity_ = {0,0};
                                    platform->entity_shape_->position_ = previous_position;
                                    other_enemy->entity_shape_->velocity_.y = 0;
                                    other_enemy->entity_shape_->velocity_.x = previous_enemy_velocity.x;
                                }

                            }



                        }

                        // handle the collision between the enemy and the platrform
                        else if(enemy_with_player )
                        {
                            if(enemy && other_player)
                            {
                                if(enemy->entity_shape_->Collides(other_player->entity_shape_))
                                {
                                    current_game_state_= kDead;
                                }
                            }
                            else if (other_enemy && player)
                            {
                                if(other_enemy->entity_shape_->Collides(player->entity_shape_))
                                {
                                    current_game_state_= kDead;
                                }
                            }

                        }
                        
                        if(player_with_win_platform)
                        {
                            if(player && other_win_platform )
                            {
                                bool collides = player->entity_shape_->Collides(other_win_platform->entity_shape_);
                                if(collides)
                                {
                                    current_game_state_ = kWin;
                                }

                            }

                            if(other_player && win_platform)
                            {
                                bool collides = win_platform->entity_shape_->Collides(other_player->entity_shape_);
                                if(collides)
                                {
                                    current_game_state_ = kWin;
                                }
                            }

                        }
                    }
                }


                // check if player fell off:
                if(player_->entity_shape_->position_.y > GetScreenHeight()/2.f + current_level_->GetLevelCamera()->offset.y)
                {
                    current_game_state_ = kDead;
                }

                auto level_camera = current_level_->GetLevelCamera();
                if(IsKeyDown(KEY_LEFT))
                {
                    player_->entity_shape_->velocity_.x = -100;
                }
                if(IsKeyDown(KEY_RIGHT))
                {
                    
                    player_->entity_shape_->velocity_.x = 100;
                }
                if(IsKeyDown(KEY_UP))
                {
                    player_->Jump();

                }
                
                if(IsKeyPressed(KEY_M))
                {
                    // easter egg for changing player shape mid game
                    if(auto circle = std::dynamic_pointer_cast<Circle>(player_->entity_shape_))
                    {
                        SetPlayerShape(
                            std::make_shared<Rect>(0,0,0,0,30,30,RED,false)
                        );
                    }
                    else if(auto rect = std::dynamic_pointer_cast<Rect>(player_->entity_shape_))
                    {
                        SetPlayerShape(
                            std::make_shared<Circle>(0,0,0,0,20,RED,false)
                        );

                    }
                }
                
                if(
                    IsKeyUp(KEY_UP) &&
                    IsKeyUp(KEY_LEFT) &&
                    IsKeyUp(KEY_RIGHT)
                )
                {
                    // set the velocity of player to zero
                    player_->entity_shape_->velocity_.x = 0;
                }


                level_camera->target.x = player_->entity_shape_->position_.x;
                
                // update what we see on the screen
                ClearBackground(SKYBLUE);
                BeginMode2D(*current_level_->GetLevelCamera());
                
                    // Update all the entity states:
                    for(auto entity: all_game_entities)
                    {
                        entity->Update();
                    }

                EndMode2D();


            }
            break;
            case kDead:
            {
                // update what we see on the screen
                ClearBackground(GRAY);
                Text game_over = {"Game Over",100,RAYWHITE};
                Text restart_instruction = {"Press space to restart.",50,RAYWHITE};
                
                game_over.DrawTextOnScreen(GetScreenWidth()/2.f,GetScreenHeight()/2.f);
                restart_instruction.DrawTextOnScreen(GetScreenWidth()/2.f,GetScreenHeight()*2.f/3.f);

                if(IsKeyPressed(KEY_SPACE))
                {
                    StartGame();
                }
            }
            break;
            case kWin:
            {
                ClearBackground(GOLD);
                Text congrats = {"Congradulations you won!",50,RAYWHITE};
                congrats.DrawTextOnScreen(GetScreenWidth()/2.f,GetScreenHeight()/3.f);

                Text next_instruction = {"To play again press SPACE.",25,RAYWHITE};
                next_instruction.DrawTextOnScreen(GetScreenWidth()/2.f,GetScreenHeight()*2.f/3.f);

                if(IsKeyPressed(KEY_SPACE))
                {
                    StartGame();
                }

            }
            break;
        }
        

    }


private:
    std::shared_ptr<Level> current_level_;
    std::shared_ptr<Player> player_;
    GameState current_game_state_;
};

#endif //GAME_H