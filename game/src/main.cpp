#include "rlImGui.h"
#include "raylib.h"
#include "Math.h"
#include "Fish.h"
#include "Rigidbody.h"
#include <vector>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

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