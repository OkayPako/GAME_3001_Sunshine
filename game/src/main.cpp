#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

Vector2 WraparoundScreen(Vector2 position)
{
    Vector2 outPosition =
    {
        (int)(position.x + SCREEN_WIDTH) % SCREEN_WIDTH,
        (int)(position.y + SCREEN_HEIGHT) % SCREEN_HEIGHT
    };

    return outPosition;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    SetTargetFPS(200);

    rlImGuiSetup(true); // Sets up imgui

    Vector2 position = { 100, 100 };
    Vector2 urMom = { 100, 400 };
    Vector2 velocity = { 10, 0 };
    Vector2 acceleration = { 0, 50 };
    float maxSpeed = 1000;

    bool useGUI = false;
    while (!WindowShouldClose())
    {
        const float deltaTime = GetFrameTime();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsKeyDown(KEY_GRAVE)) useGUI = !useGUI;
        if (useGUI)
        {
            rlImGuiBegin();
            
            ImGui::SliderFloat2("Position", &(position.x), 0, SCREEN_WIDTH);
            ImGui::SliderFloat2("Velocity", &(velocity.x), -maxSpeed, maxSpeed);
            ImGui::SliderFloat2("Acceleration", &(acceleration.x), -maxSpeed, maxSpeed);

            rlImGuiEnd();
        }

        Vector2 displacement = velocity * deltaTime; // px/s * s = px
        position = position + displacement;

        Vector2 deltaV = acceleration * deltaTime;
        velocity = velocity + deltaV; // px/s/s/s * s = px/s

        position = position + displacement * acceleration * deltaTime * deltaTime * 0.5f;

        position = WraparoundScreen(position);

        DrawText("Hello World!", 16, 9, 20, RED);

        DrawCircleV(position, 50, GREEN);
        DrawCircleGradient(urMom.x, urMom.y, 50, LIGHTGRAY, BLACK);

        
        DrawFPS(10, 100);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}