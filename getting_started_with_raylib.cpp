#include "raylib.h"

// first put the class in the same file with the main loop

class Circle
{
public:
    // add a constructor because we want to customize our initial circle appearance.
    Circle(
        float initial_x,float initial_y,
        float initial_x_vel,float initial_y_vel,
        float initial_x_acc,float initial_y_acc,
        float radius, Color color
    )
    {
        position_.x = initial_x;
        position_.y = initial_y;

        velocity_.x = initial_x_vel;
        velocity_.y = initial_y_vel;

        acceleration_.x = initial_x_acc;
        acceleration_.y = initial_y_acc;

        color_ = color;
        radius_ = radius;
    }

    void Draw()
    {
        // update the circle's position based on velocity and acceleration
        
        DrawCircle();
    }
    

private:
    Vector2 position_;
    Vector2 velocity_;
    Vector2 acceleration_;
    Color color_;
    float radius_;
};

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            // TODO: Draw a circle on screen
            // DrawCircle(400,225,10,RED);

            // TODO: make the circle move by turning into a class with velocity and position and acceleration
            

            // TODO: make the cricle collide with the screen

            // TODO: make many circles that go in different directions

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}