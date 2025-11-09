#include "raylib.h"
#include "raymath.h"


int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        
        // TODO: implement the entity classes that use the shapes
        
        // TODO: implement Level and game class

        EndDrawing();
    }

    CloseWindow();

}