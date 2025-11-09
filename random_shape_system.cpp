#include "raylib.h"
#include "raymath.h"
#include <format>
#include <string>
#include <vector>
#include <iostream>
#include "shapes.h"
#include <memory>

std::shared_ptr<Circle> GenerateCricle()
{
    float rand_init_x = GetRandomValue(0,GetScreenWidth());
    float rand_init_y = GetRandomValue(0,GetScreenHeight());

    float rand_vel_x = GetRandomValue(50,200);
    float rand_vel_y = GetRandomValue(50,200);
    // to cover the whole 360 we need to have a chance of having negative velocities
    int rand_vel_x_negative = GetRandomValue(0,1) == 0 ? -1 : 1;
    int rand_vel_y_negative = GetRandomValue(0,1) == 0 ? -1 : 1;
    
    float rand_radius = GetRandomValue(5,50);

    return std::make_shared<Circle>(
            rand_init_x,rand_init_y,
            rand_vel_x*rand_vel_x_negative,rand_vel_y*rand_vel_y_negative,
            rand_radius,RED);
}

std::shared_ptr<Rect> GenerateRect()
{
    float rand_init_x = GetRandomValue(0,GetScreenWidth());
    float rand_init_y = GetRandomValue(0,GetScreenHeight());

    float rand_vel_x = GetRandomValue(50,200);
    float rand_vel_y = GetRandomValue(50,200);
    // to cover the whole 360 we need to have a chance of having negative velocities
    int rand_vel_x_negative = GetRandomValue(0,1) == 0 ? -1 : 1;
    int rand_vel_y_negative = GetRandomValue(0,1) == 0 ? -1 : 1;
    
    float rand_width = GetRandomValue(5,50);
    float rand_height = GetRandomValue(5,50);

    return std::make_shared<Rect>(
            rand_init_x,rand_init_y,
            rand_vel_x*rand_vel_x_negative,rand_vel_y*rand_vel_y_negative,
            rand_width,rand_height,GREEN);
}


int main(void)
{
    InitWindow(800, 450, "raylib [core] example - basic window");
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    SetWindowState(FLAG_WINDOW_RESIZABLE|FLAG_WINDOW_HIGHDPI);
    SetTargetFPS(60);

    bool initialize = true;

    std::vector<std::shared_ptr<Shape>> shapes;


    while (!WindowShouldClose())
    {
        if(initialize)
        {
            shapes.clear();
            const int num_shapes =30;
            for(int i=0;i<num_shapes;i++)
            {
                shapes.push_back(GenerateCricle());
            }

            for(int i=0;i<num_shapes;i++)
            {
                shapes.push_back(GenerateRect());
            }
            
            initialize = false;  
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            // trigger on new frame event
            for(auto shape : shapes)
            {
                shape->OnNewFrame();
            }
            
            // compute shape collisions
            for(auto shape : shapes)
            {
                for(auto other : shapes)
                {
                    shape->Collides(other);
                }
            }

            for(auto shape:shapes)
            {
                shape->Update();
            }


            if(IsKeyDown(KEY_SPACE))
            {
                initialize = true;
            }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}