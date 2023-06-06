#include "rlImGui.h"
#include "raylib.h"
#include "Math.h"
#include "Fish.h"
#include "Rigidbody.h"
#include <vector>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

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