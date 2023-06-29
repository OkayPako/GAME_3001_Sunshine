#include "rlImGui.h"
#include "raylib.h"
#include "TileCoord.h"
#include "Tilemap.h"
#include "Pathfinder.h"
#include "Math.h"
#include <time.h>
#include <queue>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 768
#define PLAYER_SIZE 64

TileCoord LerpTileCoord(const TileCoord& start, const TileCoord& end, float t)
{
    float x = Lerp(start.x, end.x, t);
    float y = Lerp(start.y, end.y, t);
    return TileCoord(static_cast<int>(x), static_cast<int>(y));
}

int main(void)
{
    srand(time(NULL));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Randomize Maps");
    rlImGuiSetup(true);

    // Player
    Texture2D playerTexture = LoadTexture("../game/assets/textures/Biker_idle.png");
    Tilemap map(playerTexture);

    map.Randomize();

    bool useGUI = false;
    bool useTextures = false;
    bool showInfo = false;
    bool showGPS = false;

    SetTargetFPS(60);

    // Pathfinder stuff
    Pathfinder pathfinder(&map, map.playerPosition, TileCoord());

    pathfinder.startNode = map.playerPosition;
    map.DrawStartNode(pathfinder.startNode);

    Vector2 mousePosition = GetMousePosition();
    TileCoord clickedTile = map.GetTileAtScreenPosition(mousePosition);
    pathfinder.goalNode = clickedTile;
    map.DrawGoalNode(clickedTile);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        map.Update(deltaTime);
        map.Draw();
        map.DrawPlayer();
        map.DrawBorders();

        // Player movement
        if (IsKeyPressed(KEY_W) && map.IsTileTraversable(map.playerPosition - TileCoord(0, 1)))
            map.playerPosition -= TileCoord(0, 1);
        if (IsKeyPressed(KEY_S) && map.IsTileTraversable(map.playerPosition + TileCoord(0, 1)))
            map.playerPosition += TileCoord(0, 1);
        if (IsKeyPressed(KEY_A) && map.IsTileTraversable(map.playerPosition - TileCoord(1, 0)))
            map.playerPosition -= TileCoord(1, 0);
        if (IsKeyPressed(KEY_D) && map.IsTileTraversable(map.playerPosition + TileCoord(1, 0)))
            map.playerPosition += TileCoord(1, 0);

        // Secondary player movement with left mouse button
        //if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        //{
        //    Vector2 mousePosition = GetMousePosition();
        //    TileCoord clickedTile = map.GetTileAtScreenPosition(mousePosition);
        //
        //    if (map.IsTileTraversable(clickedTile))
        //        map.playerPosition = clickedTile;
        //}

        if (IsKeyPressed(KEY_GRAVE)) useGUI = !useGUI;
        if (useGUI)
        {
            rlImGuiBegin();

            if (ImGui::Checkbox("Use Textures", &useTextures))
            {
                map.SetUseTextures(useTextures);
            }

            if (ImGui::Checkbox("Draw Tile Information", &showInfo))
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
            map.DrawTileInfo();
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