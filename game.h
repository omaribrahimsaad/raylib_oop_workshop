#ifndef GAME_H
#define GAME_H
#include "raylib.h"
#include "raymath.h"
#include <memory>
#include "game_entities.h"
#include <vector>
#include <map>


/*
    Level class will help us to store all the game enitites and the
    player starting position.
*/
class Level
{
public:
    Level(std::shared_ptr<Camera2D> camera,Vector2 player_starting_position)
    {
        camera_ = camera;
        player_starting_point_ = player_starting_position;
    }   
    
    std::vector<std::shared_ptr<Entity>> level_entities_;
    std::shared_ptr<Camera2D> camera_;
    Vector2 player_starting_point_;

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
    Game(std::shared_ptr<Player> player,std::shared_ptr<Level> initial_level)
    {
        player_ = player;
        current_level_ = initial_level;

        initial_level->level_entities_.push_back(player);
    }

    void SetLevel(std::shared_ptr<Level> level)
    {
        current_level_ = level;
        level->level_entities_.push_back(player_);
    }

    void StartGame()
    {
        current_level_->camera_->target = current_level_->player_starting_point_;
        player_->entity_shape_->position_ = current_level_->player_starting_point_;
        current_game_state_ = kPlaying;
    }

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
                //-------------------------------------------------------------------------------------------------
                // collision handling
                //-------------------------------------------------------------------------------------------------
                for(int i=0;i<current_level_->level_entities_.size();i++)
                {
                    for(int j=i+1;j< current_level_->level_entities_.size();j++)
                    {
                        // first we need to know which entity is the one we are using first
                        {

                            auto player = std::dynamic_pointer_cast<Player>(current_level_->level_entities_[i]);
                            auto platform = std::dynamic_pointer_cast<Platform>(current_level_->level_entities_[j]);
                            
                            if(!player || !platform)
                            {
                                player = std::dynamic_pointer_cast<Player>(current_level_->level_entities_[j]);
                                platform = std::dynamic_pointer_cast<Platform>(current_level_->level_entities_[i]);
                                
                            }
                            
                            // Now check if we have a valid player-platform pair
                            if (player && platform)
                            {
                                auto prev_platform_position = platform->entity_shape_->position_;
                                if (player->entity_shape_->Collides(platform->entity_shape_))
                                {
                                    // Platform stays fixed
                                    platform->entity_shape_->velocity_ = {0, 0};
                                    platform->entity_shape_->position_ = prev_platform_position;
                                    player_->can_jump_ = true;
                                }
                            }                    
                        }

                        {

                            auto enemy = std::dynamic_pointer_cast<Enemy>(current_level_->level_entities_[i]);
                            auto platform = std::dynamic_pointer_cast<Platform>(current_level_->level_entities_[j]);
                            
                            if(!enemy || !platform)
                            {
                                enemy = std::dynamic_pointer_cast<Enemy>(current_level_->level_entities_[j]);
                                platform = std::dynamic_pointer_cast<Platform>(current_level_->level_entities_[i]);
                            }
                            
                            if(enemy && platform)
                            {
                                auto prev_platform_position = platform->entity_shape_->position_;
                                if (enemy->entity_shape_->Collides(platform->entity_shape_))
                                {
                                    // Platform stays fixed
                                    platform->entity_shape_->velocity_ = {0, 0};
                                    platform->entity_shape_->position_ = prev_platform_position;
                                    
                                }
                                
                            }
                        }
                            
                        {
                            // collision with enemy
                            auto enemy = std::dynamic_pointer_cast<Enemy>(current_level_->level_entities_[j]);
                            auto player = std::dynamic_pointer_cast<Player>(current_level_->level_entities_[i]);
                            // swap if neccessary
                            if(!enemy || !player)
                            {
                                enemy = std::dynamic_pointer_cast<Enemy>(current_level_->level_entities_[i]);
                                player = std::dynamic_pointer_cast<Player>(current_level_->level_entities_[j]);
                            }
                            
                            if(enemy && player)
                            {
                                if (enemy->entity_shape_->Collides(player->entity_shape_))
                                {
                                    current_game_state_ = kDead;   
                                }
                                
                            }
                        }

                        
                        {
                            // colision with win platform
                            auto win_platform = std::dynamic_pointer_cast<WinPlatform>(current_level_->level_entities_[i]);
                            auto player = std::dynamic_pointer_cast<Player>(current_level_->level_entities_[j]);

                            if(!win_platform || !player)
                            {
                                auto win_platform = std::dynamic_pointer_cast<WinPlatform>(current_level_->level_entities_[j]);
                                auto player = std::dynamic_pointer_cast<Player>(current_level_->level_entities_[i]);
                            }

                            if(win_platform && player)
                            {
                                auto prev_platform_position = win_platform->entity_shape_->position_;
                                if (player->entity_shape_->Collides(win_platform->entity_shape_))
                                {
                                    // Platform stays fixed
                                    win_platform->entity_shape_->velocity_ = {0, 0};
                                    win_platform->entity_shape_->position_ = prev_platform_position;
                                    current_game_state_ = kWin;
                                }

                            }
                        }


                    }
                }
                
                // handle if player falls:
                if(player_->entity_shape_->position_.y > current_level_->camera_->target.y + GetScreenHeight()/2)
                {
                    current_game_state_ = kDead;
                }

                //-------------------------------------------------------------------------------------------------
                // input handling
                //-------------------------------------------------------------------------------------------------
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
                    player_->can_jump_ = false;
                }
                                
                // is to avoid any movement when we are not pressing any key
                if(
                    IsKeyUp(KEY_UP) &&
                    IsKeyUp(KEY_LEFT) &&
                    IsKeyUp(KEY_RIGHT)
                )
                {
                    // set the velocity of player to zero
                    player_->entity_shape_->velocity_.x = 0;
                }
                
                current_level_->camera_->target.x = player_->entity_shape_->position_.x;

                //-------------------------------------------------------------------------------------------------
                // core game loop
                //-------------------------------------------------------------------------------------------------
                ClearBackground(SKYBLUE);
                BeginMode2D(*current_level_->camera_);
                
                    // Update all the entity states:
                    for(auto entity: current_level_->level_entities_)
                    {
                        entity->Update();
                    }

                    player_->Update();

                EndMode2D();


            }
            break;
            case kDead:
            {
                ClearBackground(GRAY);
                Text death_message = {"You Died!",100,WHITE};
                death_message.DrawTextOnScreen(GetScreenWidth()/2.f,GetScreenHeight()/3.f);

                Text restart_instruction = {"Press space to restart",50,WHITE};
                restart_instruction.DrawTextOnScreen(GetScreenWidth()/2.f,2.f*GetScreenHeight()/3.f);

                if(IsKeyPressed(KEY_SPACE))
                {
                    player_->entity_shape_->position_ = current_level_->player_starting_point_;
                    current_game_state_ = kPlaying;
                }
            }
            break;
            case kWin:
            {
                ClearBackground(GOLD);
                Text death_message = {"You Won!",100,WHITE};
                death_message.DrawTextOnScreen(GetScreenWidth()/2.f,GetScreenHeight()/3.f);

                Text restart_instruction = {"Press space to restart",50,WHITE};
                restart_instruction.DrawTextOnScreen(GetScreenWidth()/2.f,2.f*GetScreenHeight()/3.f);

                if(IsKeyPressed(KEY_SPACE))
                {
                    player_->entity_shape_->position_ = current_level_->player_starting_point_;
                    current_game_state_ = kPlaying;
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