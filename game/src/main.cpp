#include "rlImGui.h"
#include <time.h>
#include"Tilemap.h"


#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720


Tilemap map;


int main(void)
{
    srand(time(NULL));
    map.Randomize();
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    rlImGuiSetup(true);

    bool useGUI = false;
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {

        map.Draw();
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsKeyPressed(KEY_GRAVE)) useGUI = !useGUI;
        if (useGUI)
        {
            rlImGuiBegin();
            if (ImGui::Button("Randomize Map"))
            {
                map.Randomize();
            }
            rlImGuiEnd();
        }

        DrawText("Hello World!", 16, 9, 20, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}