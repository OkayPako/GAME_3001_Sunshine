#include "rlImGui.h"
#include "raylib.h"
#include "Math.h"
#include <vector>
#include <iostream>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

struct Rigidbody
{
    Vector2 pos{};          // Position
    Vector2 vel{};          // Velocity
    Vector2 acc{};          // Acceleration

    Vector2 dir{};          // Direction
    float angularSpeed;     // Angular speed (in degrees per second)
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
        rigidbody.dir = { 1, 0 }; // Initial direction
        rigidbody.angularSpeed = 0.0f; // Initial angular speed
    }


    void Update(float deltaTime)
    {
        // Calculate desired velocity based on direction and maximum speed
        Vector2 desiredVel = Scale(rigidbody.dir, maxSpeed);

        // Calculate steering force based on desired velocity and current velocity
        Vector2 steering = Subtract(desiredVel, rigidbody.vel);

        // Limit the steering force to the maximum swim acceleration
        steering = Clamp(steering, -maxAcceleration, maxAcceleration);

        // Apply the steering force to the fish's acceleration
        rigidbody.acc = steering;

        // Update velocity based on acceleration
        rigidbody.vel.x += rigidbody.acc.x * deltaTime;
        rigidbody.vel.y += rigidbody.acc.y * deltaTime;

        // Limit velocity to the maximum swim speed
        rigidbody.vel = Clamp(rigidbody.vel, -maxSpeed, maxSpeed);

        // Update position based on velocity
        rigidbody.pos.x += rigidbody.vel.x * deltaTime;
        rigidbody.pos.y += rigidbody.vel.y * deltaTime;

        // Update direction based on velocity
        if (rigidbody.vel.x != 0 || rigidbody.vel.y != 0)
        {
            rigidbody.dir = Normalize(rigidbody.vel);
        }

        // Update sprite rotation based on direction
        float rotation = atan2f(rigidbody.dir.y, rigidbody.dir.x) * RAD2DEG;
        rigidbody.angularSpeed = rotation;

        // Check screen bounds
        if (rigidbody.pos.x < 0)
        {
            rigidbody.pos.x = 0;
            rigidbody.vel.x = 0;
        }
        else if (rigidbody.pos.x > SCREEN_WIDTH - width)
        {
            rigidbody.pos.x = SCREEN_WIDTH - width;
            rigidbody.vel.x = 0;
        }

        if (rigidbody.pos.y < 0)
        {
            rigidbody.pos.y = 0;
            rigidbody.vel.y = 0;
        }
        else if (rigidbody.pos.y > SCREEN_HEIGHT - height)
        {
            rigidbody.pos.y = SCREEN_HEIGHT - height;
            rigidbody.vel.y = 0;
        }
    }

    void Draw() const
    {
        Rectangle sourceRect = { 0, 0, (float)texture.width, (float)texture.height };
        Rectangle destRect = { rigidbody.pos.x, rigidbody.pos.y, width, height };
        Vector2 origin = { width / 2.0f, height / 2.0f };

        DrawTexturePro(texture, sourceRect, destRect, origin, rigidbody.angularSpeed, WHITE);
    }
};

// Calculates a steering force that directs the agent towards a target position while limiting its maximum speed.
Vector2 Seek(const Vector2& pos, const Rigidbody& rigidbody, float maxSpeed)
{
    return Normalize(pos - rigidbody.pos) * maxSpeed - rigidbody.vel;
}

// Calculates a steering force that directs a target position towards the agent while limiting its maximum speed.
Vector2 Flee(const Vector2& pos, const Rigidbody& rigidbody, float maxSpeed)
{
    return Normalize(rigidbody.pos - pos) * maxSpeed - rigidbody.vel;
}

// Checks if a line segment intersects with a obstacle, which is used later to detect collisions between the seeker and an obstacle.
bool CheckCollisionLineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius)
{
    Vector2 nearest = NearestPoint(lineStart, lineEnd, circlePosition);
    return DistanceSqr(nearest, circlePosition) <= circleRadius * circleRadius;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fishies");
    SetTargetFPS(60);

    // Load background
    Texture2D background = LoadTexture("../game/assets/textures/sea.png");
    background.width = 1920;
    background.height = 1080;

    // Fishes
    std::vector<Fish> fishies;
    Texture2D fishPic = LoadTexture("../game/assets/textures/magikarp.png");
    Fish fish1({ 400,400 }, fishPic, 50, 50, 200.0f, 400.0f);
    Fish fish2({ 800,400 }, fishPic, 50, 50, 400.0f, 800.0f);
    fishies.push_back(fish1);
    fishies.push_back(fish2);

    while (!WindowShouldClose())
    {
        // Update
        float deltaTime = GetFrameTime();


        int choice;
        std::cin >> choice;

        BeginDrawing();
        ClearBackground(SKYBLUE);

        // Draw Background
        DrawTexture(background, 0, 0, RAYWHITE);

        // Draw agents
        for (const Fish& agent : fishies)
        {
            agent.Draw();
        }

        EndDrawing();
    }

    // Unload resources
    UnloadTexture(background);
    for (const Fish& agent : fishies)
    {
        UnloadTexture(agent.texture);
    }

    CloseWindow();
    return 0;
}