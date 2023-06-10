//Brandon Quach-101191483
//Raphael Flores-101139370

#include "raylib.h"
#include "Math.h"
#include <cmath>
#include <vector>

struct Rigidbody
{
    Vector2 pos{};
    Vector2 vel{};
    Vector2 acc{};
    Vector2 dir{};
    float angularSpeed{};
};

struct Obstacle
{
    Vector2 position{};
    float radius{};
};

struct Food
{
    Vector2 position{};
    float radius{};
};

class Fish
{
public:
    Rigidbody rigidbody;
    Texture2D texture;
    float width;
    float height;
    float maxSpeed;
    float maxAcceleration;

    Fish(const Vector2& position, const Texture2D& texture, float width, float height, float speed, float acceleration)
        : maxSpeed(speed)
        , maxAcceleration(acceleration)
    {
        this->texture = texture;
        this->width = width;
        this->height = height;
        rigidbody.pos = position;
        rigidbody.vel = { 0, 0 };
        rigidbody.dir = { 1, 0 };
        rigidbody.angularSpeed = 0.0f;
    }

    void Update(float deltaTime)
    {
        // Update velocity based on acceleration
        rigidbody.vel = rigidbody.vel + (rigidbody.acc * deltaTime);

        // Limit velocity to the maximum swim speed
        float speed = Length(rigidbody.vel);
        if (speed > maxSpeed)
        {
            rigidbody.vel = Normalize(rigidbody.vel) * maxSpeed;
        }

        // Update position based on velocity
        rigidbody.pos = rigidbody.pos + (rigidbody.vel * deltaTime);

        // Update direction based on velocity
        if (speed > 0)
        {
            rigidbody.dir = Normalize(rigidbody.vel);
        }

        // Update sprite rotation based on direction
        rigidbody.angularSpeed = atan2f(rigidbody.dir.y, rigidbody.dir.x) * RAD2DEG;

        // Clear acceleration for the next frame
        rigidbody.acc = { 0, 0 };
    }

    void Draw() const
    {
        Rectangle sourceRect = { 0, 0, (float)texture.width, (float)texture.height };
        Rectangle destRect = { rigidbody.pos.x, rigidbody.pos.y, width, height };
        Vector2 origin = { width / 2.0f, height / 2.0f };

        DrawTexturePro(texture, sourceRect, destRect, origin, rigidbody.angularSpeed, WHITE);
    }
};

float DistanceSquared(const Vector2& v1, const Vector2& v2)
{
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    return (dx * dx) + (dy * dy);
}

bool CheckCollisionLineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius)
{
    Vector2 nearest = NearestPoint(lineStart, lineEnd, circlePosition);
    return DistanceSquared(nearest, circlePosition) <= (circleRadius * circleRadius);
}

int main()
{
    const int screenWidth = 1900;
    const int screenHeight = 1000;

    InitWindow(screenWidth, screenHeight, "Fish Behaviors");
    SetTargetFPS(60);

    Texture2D sea = LoadTexture("../game/assets/textures/sea.png");
    Texture2D fishTexture = LoadTexture("../game/assets/textures/magikarp.png");

    std::vector<Fish> fishies;
    fishies.push_back(Fish({ 100, 200 }, fishTexture, 50, 50, 150.0f, 300.0f));
    fishies.push_back(Fish({ 200, 300 }, fishTexture, 50, 50, 200.0f, 400.0f));
    fishies.push_back(Fish({ 300, 400 }, fishTexture, 50, 50, 250.0f, 500.0f));

    std::vector<Obstacle> obstacles;

    std::vector<Food> foods;

    Vector2 targetPosition{};
    bool seekMode = false;
    bool fleeMode = false;
    bool arriveMode = false;
    bool avoidMode = false;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        // Update
        float deltaTime = GetFrameTime();

        // Handle input
        if (IsKeyPressed(KEY_SPACE))
        {
            // Reset the simulation
            fishies.clear();
            fishies.push_back(Fish({ 100, 200 }, fishTexture, 50, 50, 150.0f, 300.0f));
            fishies.push_back(Fish({ 200, 300 }, fishTexture, 50, 50, 200.0f, 400.0f));
            fishies.push_back(Fish({ 300, 400 }, fishTexture, 50, 50, 250.0f, 500.0f));
            seekMode = false;
            fleeMode = false;
            arriveMode = false;
            avoidMode = false;
        }
        if (IsKeyPressed(KEY_ONE))
        {
            seekMode = true;
            fleeMode = false;
            arriveMode = false;
            avoidMode = false;
        }
        else if (IsKeyPressed(KEY_TWO))
        {
            seekMode = false;
            fleeMode = true;
            arriveMode = false;
            avoidMode = false;
        }
        else if (IsKeyPressed(KEY_THREE))
        {
            seekMode = false;
            fleeMode = false;
            arriveMode = true;
            avoidMode = false;
        }
        else if (IsKeyPressed(KEY_FOUR))
        {
            seekMode = false;
            fleeMode = false;
            arriveMode = false;
            avoidMode = true;
        }

        if (IsMouseButtonUp(MOUSE_LEFT_BUTTON))
        {
            if (seekMode)
            {
                targetPosition = GetMousePosition();
            }
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            if (avoidMode)
            {
                Obstacle obstacle;
                obstacle.position = GetMousePosition();
                obstacle.radius = 10;
                obstacles.push_back(obstacle);
            }
        }

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            if (arriveMode)
            {
                Food food;
                food.position = GetMousePosition();
                food.radius = 5;
                foods.push_back(food);

            }
        }


        // Behaviors
        for (Fish& fish : fishies)
        {
            // Seek behavior
            if (seekMode)
            {
                Vector2 desiredVel = targetPosition - fish.rigidbody.pos;
                desiredVel = Normalize(desiredVel) * fish.maxSpeed;
                Vector2 steering = desiredVel - fish.rigidbody.vel;
                fish.rigidbody.acc = fish.rigidbody.acc + steering;
            }

            // Flee behavior
            if (fleeMode)
            {
                Vector2 predatorPosition = GetMousePosition();
                Vector2 desiredVel = fish.rigidbody.pos - predatorPosition;
                desiredVel = Normalize(desiredVel) * fish.maxSpeed;
                Vector2 steering = desiredVel - fish.rigidbody.vel;
                fish.rigidbody.acc = fish.rigidbody.acc + steering;
            }

            // Arrive behavior
            if (arriveMode)
            {
                for (const Food& food : foods)
                {
                    Vector2 desiredVel = food.position - fish.rigidbody.pos;
                    float distance = Length(desiredVel);
                    float slowRadius = 100.0f;
                    float arriveRadius = 25.0f;

                    if (distance > slowRadius)
                    {
                        desiredVel = Normalize(desiredVel) * fish.maxSpeed;
                    }
                    else if (distance > arriveRadius)
                    {
                        float t = distance / slowRadius;
                        desiredVel = Normalize(desiredVel) * fish.maxSpeed * t;
                    }
                    else
                    {
                        desiredVel = { 0, 0 };
                    }

                    Vector2 steering = desiredVel - fish.rigidbody.vel;
                    fish.rigidbody.acc = fish.rigidbody.acc + steering;
                }


            }
            // Obstacle avoidance behavior
            if (avoidMode)
            {
                targetPosition = GetMousePosition();
                Vector2 desiredVel = targetPosition - fish.rigidbody.pos;
                desiredVel = Normalize(desiredVel) * fish.maxSpeed;
                Vector2 steering = desiredVel - fish.rigidbody.vel;
                fish.rigidbody.acc = fish.rigidbody.acc + steering;
            


                for (const Obstacle& obstacle : obstacles)
                {
                    Vector2 desiredVel = fish.rigidbody.pos - obstacle.position;
                    float distance = Length(desiredVel);
                    float avoidRadius = 50.0f;

                    if (distance < avoidRadius)
                    {
                        desiredVel = Normalize(desiredVel) * fish.maxSpeed;
                        Vector2 steering = desiredVel - fish.rigidbody.vel;
                        fish.rigidbody.acc = fish.rigidbody.acc + steering;
                    }
                }
            }

            // Update fish
            fish.Update(deltaTime);

            // Wrap around screen
            if (fish.rigidbody.pos.x > screenWidth)
                fish.rigidbody.pos.x = 0;
            else if (fish.rigidbody.pos.x < 0)
                fish.rigidbody.pos.x = screenWidth;

            if (fish.rigidbody.pos.y > screenHeight)
                fish.rigidbody.pos.y = 0;
            else if (fish.rigidbody.pos.y < 0)
                fish.rigidbody.pos.y = screenHeight;
        }

        DrawTexture(sea, 0, 0, RAYWHITE);

        // Draw
        for (const Fish& fish : fishies)
        {
            fish.Draw();
        }

        // Draw target
        if (seekMode )
        {
            DrawCircleV(GetMousePosition(), 5, GREEN);
        }

        // Draw obstacles
        if (avoidMode)
        {
            for (const Obstacle& obstacle : obstacles)
            {
                DrawCircle(obstacle.position.x, obstacle.position.y, obstacle.radius, RED);
            }
        }

        if ( arriveMode)
        { 
            for (const Food& food : foods)
            {
                DrawCircle(food.position.x, food.position.y, food.radius, ORANGE);
            }
        
        }

        DrawText("Press 1 for Seek ", 20, 20, 15, RED);
        DrawText("Press 2 for Flee ", 20, 40, 15, RED);
        DrawText("Press 3 for Arrive ", 20, 60, 15, RED);
        DrawText("Press 4 for Obsticle Avoidance ", 20, 80, 15, RED);
        DrawText("Press Spacebar to reset ", 20, 100, 15, RED);

        EndDrawing();
    }
    UnloadTexture(fishTexture);
    CloseWindow();

    return 0;
}
