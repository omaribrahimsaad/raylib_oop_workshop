#include "raylib.h"
#include "raymath.h"
#include "game.h"
#include "shapes.h"
#include <iostream>
#include <format>
#include <fstream>
#include <sstream>


std::shared_ptr<Level> LoadLevelFromCSV(const std::string& filename, std::shared_ptr<Camera2D> camera)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        TraceLog(LOG_ERROR, "Failed to open CSV file: %s", filename.c_str());
        return nullptr;
    }

    std::vector<std::shared_ptr<Platform>> platforms;
    std::vector<std::shared_ptr<Enemy>> enemies;
    std::shared_ptr<WinPlatform> winPlatform = nullptr;

    std::string line;
    int row = 0;
    const float horizontal_tile_size = 100; // adjust as needed
    const float vertical_tile_size = 40;
    const float player_size = 20;

    Vector2 playerStart = {0,0};

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        int col = 0;
        while (std::getline(ss, cell, ',')) {
            if (cell.empty()) { col++; continue; }

            float x = camera->offset.x + col * horizontal_tile_size;
            float y = camera->offset.y + row * vertical_tile_size /**-1*/; // flip if your Y grows downward

            if (cell == "#") {
                platforms.push_back(std::make_shared<Platform>(
                    std::make_shared<Rect>(x, y, 0, 0, horizontal_tile_size, vertical_tile_size, GREEN, false)
                ));
            } else if (cell == "E") {
                enemies.push_back(std::make_shared<Enemy>(
                    std::make_shared<Circle>(x + horizontal_tile_size/2, y + vertical_tile_size/2, 0, 0, player_size, BLUE, false),
                    PathPlan{20, 150}
                ));
            } else if (cell == "W") {
                winPlatform = std::make_shared<WinPlatform>(
                    std::make_shared<Rect>(x, y, 0, 0, horizontal_tile_size, vertical_tile_size, GOLD, false)
                );
            } else if (cell == "P") {
                playerStart = {x + horizontal_tile_size/2, y + vertical_tile_size/2};
            }
            col++;
        }
        row++;
    }

    file.close();

    if (winPlatform) platforms.push_back(winPlatform);
    std::shared_ptr<Level> level = std::make_shared<Level>(camera, playerStart);
    level->AddPlatforms(platforms);
    level->AddEnemies(enemies);

    return level;
}

int main()
{

    InitWindow(800, 600, "My Game");
    SetTargetFPS(60);
    
    // prepare the level
    std::shared_ptr<Camera2D> test_camera = std::make_shared<Camera2D>();
    *test_camera = {0};
    test_camera->target = {0,0};
    test_camera->offset = {GetScreenWidth()/2.f,GetScreenHeight()/2.f};
    test_camera->rotation = 0;
    test_camera->zoom = 1;


    std::shared_ptr<Level> level = std::make_shared<Level>(test_camera,Vector2{0,20});
    level->AddPlatforms(
        {
            std::make_shared<Platform>(std::make_shared<Rect>(-100,200,0,0,2000,40,GREEN,false)),
            std::make_shared<Platform>(std::make_shared<Rect>(200,-200,0,0,2000,40,GREEN,false)),
            std::make_shared<WinPlatform>(std::make_shared<Rect>(2100,200,0,0,2000,40,GOLD,false)),
        }
    );
    level->AddEnemies(
        {
            std::make_shared<Enemy>(std::make_shared<Circle>(500,0,0,0,20,BLUE,false),PathPlan{150,200})
        }
    );


    // prepare the game
    std::shared_ptr<Circle> player_shape = std::make_shared<Circle>(300,100,0,0,20,RED,false);
    std::shared_ptr<Player> player = std::make_shared<Player>(player_shape);
    Game game(player);
    // game.SetLevel(level);
    
    game.SetLevel(LoadLevelFromCSV("D:/workshops/oop_with_raylib/levels/test.csv",test_camera));
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