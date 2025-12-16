#include "raylib.h"
#include "raymath.h"
#include "game.h"
#include "shapes.h"
#include <iostream>
#include <format>
#include <fstream>
#include <sstream>

int main()
{

    // initialize the window
    InitWindow(800, 600, "My Game");
    SetTargetFPS(60);
    
    //--------------------------------------------------------------------
    // initalize a 2D camera
    //--------------------------------------------------------------------
    std::shared_ptr<Camera2D> test_camera = std::make_shared<Camera2D>();
    *test_camera = {0};
    test_camera->target = {0,0};
    test_camera->offset = {GetScreenWidth()/2.f,GetScreenHeight()/2.f};
    test_camera->rotation = 0;
    test_camera->zoom = 1;

    //--------------------------------------------------------------------
    // prepare our level using hardcoded values
    //--------------------------------------------------------------------
    std::shared_ptr<Level> level = std::make_shared<Level>(test_camera,Vector2{0,20}); // create our level  with camera and player position

    // create the platforms for our level
    auto platform_shape = std::make_shared<Rect>(-100,200,0,0,2000,40,GREEN,false); 
    auto platform = std::make_shared<Platform>(platform_shape);
    level->level_entities_.push_back(platform); // add the platform to our level's entity list

    // create the player and its shape
    std::shared_ptr<Circle> player_shape = std::make_shared<Circle>(300,100,0,0,20,RED,false);
    std::shared_ptr<Player> player = std::make_shared<Player>(player_shape);
    Game game(player,level);
    
    game.StartGame();

    while (!WindowShouldClose())
    {
        BeginDrawing();

        game.Update();

        EndDrawing();

    }
    
    CloseWindow();
    
    return 0;
}