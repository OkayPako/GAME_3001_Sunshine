#include "rlImGui.h"
#include "Tilemap.h"
#include "raylib.h"
#include "Pathfinder.h"
#include <time.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 768
#define PLAYER_SIZE 64

int main(void)
{
    srand(time(NULL));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Randomize Maps");
    rlImGuiSetup(true);

    // Player
    Texture2D playerTexture = LoadTexture("../game/assets/textures/Biker_idle.png");
    Tilemap map(playerTexture);
    Pathfinder pathfinder;

    map.Randomize();

    bool useGUI = false;
    bool useTextures = false;
    bool showInfo = false;
    bool showGPS = false;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        map.Update(deltaTime);
        map.Draw();
        map.DrawPlayer();
        map.DrawBorders();
        map.DrawPathfinderVisualization(pathfinder);

        // Player movement
        if (IsKeyPressed(KEY_W) && map.IsTileTraversable(map.playerPosition - TileCoord(0, 1)))
            map.playerPosition -= TileCoord(0, 1);
        if (IsKeyPressed(KEY_S) && map.IsTileTraversable(map.playerPosition + TileCoord(0, 1)))
            map.playerPosition += TileCoord(0, 1);
        if (IsKeyPressed(KEY_A) && map.IsTileTraversable(map.playerPosition - TileCoord(1, 0)))
            map.playerPosition -= TileCoord(1, 0);
        if (IsKeyPressed(KEY_D) && map.IsTileTraversable(map.playerPosition + TileCoord(1, 0)))
            map.playerPosition += TileCoord(1, 0);

        if (IsKeyPressed(KEY_GRAVE)) useGUI = !useGUI;
        if (useGUI)
        {
            rlImGuiBegin();

            if (ImGui::Checkbox("Use Textures", &useTextures))
            {
                map.SetUseTextures(useTextures);
            }

            if (ImGui::Checkbox("Draw Information", &showInfo))
            {
            }

            if (ImGui::Checkbox("Show GPS", &showGPS))
            {
            }

            if (ImGui::Button("Randomize Map"))
            {
                map.Randomize();
            }

            rlImGuiEnd();
        }

        if (showInfo)
        {
            map.DrawInformationOnTiles();
        }

        if (showGPS)
        {
            map.DrawGPS(map);
        }

        DrawRectangle( 10, 10, 310, 20, BLACK);
        DrawText("PRESS ~ TO OPEN THE MENU :)", 10, 10, 20, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}