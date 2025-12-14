#include "raylib.h"
#include "raymath.h"
#include <memory>
#include <vector>
#include <cmath>

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

    void Update()
    {
        float dt = GetFrameTime();
        if(dt > 0.05) dt = 0;
        
        // update the circle's position based on velocity and acceleration
        velocity_ += acceleration_ * dt; 
        position_ += velocity_ * dt; 
        
        HandleEdgeCollision();

        DrawCircle(position_.x,position_.y,radius_,color_);
    }

    bool Collides(std::shared_ptr<Circle> other)
    {
        // don't proceed if it is with yourself
        if(this == other.get())
            return false;
        
        
        // cast to find what is the derived type

            
        float distance = Vector2Distance(other->position_, position_);
    
        if(distance < radius_ + other->radius_ && distance > 0.0001f)
        {
            /*
                v1' = v1 - (x1-x2) * (DOT((v1-v2),(x1-x2)) / d * d)
                v2' = v2 - (x2-x1) * (DOT((v2-v1),(x2-x1)) / d * d)
            */
            // Position difference vectors
            Vector2 pos_diff_this = Vector2Subtract(position_, other->position_);
            Vector2 pos_diff_other = Vector2Subtract(other->position_, position_);
            
            // Velocity difference vectors  
            Vector2 vel_diff_this = Vector2Subtract(velocity_, other->velocity_);
            Vector2 vel_diff_other = Vector2Subtract(other->velocity_, velocity_);
            
            // Calculate dot products
            float dot_this = Vector2DotProduct(vel_diff_this, pos_diff_this);
            float dot_other = Vector2DotProduct(vel_diff_other, pos_diff_other);
            
            // Squared distance
            float dist_squared = distance * distance;
            
            // Calculate new velocities (for equal mass)
            Vector2 this_new_vel = Vector2Subtract(velocity_, 
                Vector2Scale(pos_diff_this, dot_this / dist_squared));
            Vector2 other_new_vel = Vector2Subtract(other->velocity_, 
                Vector2Scale(pos_diff_other, dot_other / dist_squared));
            
            // Check for NaN (should be && not ||)
            if(!std::isnan(this_new_vel.x) && !std::isnan(this_new_vel.y))
            {
                velocity_ = this_new_vel;

            }
            
            if(!std::isnan(other_new_vel.x) && !std::isnan(other_new_vel.y))
                other->velocity_ = other_new_vel;
            
            // Separate overlapping circles
            float overlap = (radius_ + other->radius_) - distance;
            if(overlap > 0)
            {
                Vector2 separation = Vector2Scale(pos_diff_this, overlap / (2.0f * distance));
                position_ = Vector2Add(position_, separation);
                other->position_ = Vector2Subtract(other->position_, separation);
            }
            return true;
        }
        return false;
    }
    

private:
    void HandleEdgeCollision()
    {
        if(position_.x + radius_ > GetScreenWidth())
        {
            velocity_.x = -velocity_.x;
            position_.x = GetScreenWidth() - radius_;
        }
        if(position_.x - radius_ < 0)
        {
            position_.x = radius_;
            velocity_.x = -velocity_.x;
        }

        if(position_.y + radius_ > GetScreenHeight() )
        {
            position_.y = GetScreenHeight() - radius_;
            velocity_.y = 0;
        }
        if(position_.y - radius_ <0)
        {
            position_.y =  radius_;
            velocity_.y = -velocity_.y;
        }
    }

    Vector2 position_;
    Vector2 velocity_;
    Vector2 acceleration_;
    Color color_;
    float radius_;
};

std::shared_ptr<Circle> GenerateCricle()
{
    float rand_init_x = GetRandomValue(0,GetScreenWidth());
    float rand_init_y = GetRandomValue(0,GetScreenHeight());

    float rand_vel_x = GetRandomValue(100,250);
    float rand_vel_y = GetRandomValue(100,250);
    // to cover the whole 360 we need to have a chance of having negative velocities
    int rand_vel_x_negative = GetRandomValue(0,1) == 0 ? -1 : 1;
    int rand_vel_y_negative = GetRandomValue(0,1) == 0 ? -1 : 1;
    
    float rand_radius = GetRandomValue(5,50);

    return std::make_shared<Circle>(
            rand_init_x,rand_init_y,
            rand_vel_x*rand_vel_x_negative,rand_vel_y*rand_vel_y_negative,0,0,
            rand_radius,RED);
}



int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    std::vector<std::shared_ptr<Circle>> circles;
    int circle_num = 20;
    for(int i = 0;i<circle_num;i++)
    {
        circles.push_back(GenerateCricle());
    }
    
    // Circle circle(400,225,20,20,0,0,40,RED);
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
            // circle.Update();

            // TODO: Add many circles using a vector
            for(auto circle : circles)
            {
                circle->Update();
            }
            // TODO: make the cricle collide with the screen

            // TODO: impelement circle-to-circle collisin
            for(auto circle : circles)
            {
                for(auto other : circles)
                {
                    circle->Collides(other);
                }
            }
            

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}