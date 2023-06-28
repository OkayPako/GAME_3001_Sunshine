#include "rlImGui.h"
#include "raylib.h"
#include "TileCoord.h"
#include "Tilemap.h"
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
    
    Pathfinder pathfinder(&map, { 1, 1 }, { 18, 10 });
    bool isPathSolved = pathfinder.SolvePath();

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

        // Player movement
        if (IsKeyPressed(KEY_W) && map.IsTileTraversable(map.playerPosition - TileCoord(0, 1)))
            map.playerPosition -= TileCoord(0, 1);
        if (IsKeyPressed(KEY_S) && map.IsTileTraversable(map.playerPosition + TileCoord(0, 1)))
            map.playerPosition += TileCoord(0, 1);
        if (IsKeyPressed(KEY_A) && map.IsTileTraversable(map.playerPosition - TileCoord(1, 0)))
            map.playerPosition -= TileCoord(1, 0);
        if (IsKeyPressed(KEY_D) && map.IsTileTraversable(map.playerPosition + TileCoord(1, 0)))
            map.playerPosition += TileCoord(1, 0);

        // Set end node with left mouse button
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            TileCoord mouseTile = map.GetTileAtScreenPosition(GetMousePosition());
            if (map.IsTileTraversable(mouseTile))
            {
                pathfinder.SetEndNode(mouseTile);
                isPathSolved = pathfinder.SolvePath();
                map.SetPlayerDestination(mouseTile);
            }
        }

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

        // Draw the solution path in blue
        if (isPathSolved)
        {
            TileCoord currentNode = pathfinder.goalNode;

            while (currentNode != pathfinder.startNode)
            {
                Vector2 nodePos = map.GetScreenPositionOfTile(currentNode);

                // Draw blue rectangle to represent the solution path
                DrawRectangle(static_cast<int>(nodePos.x), static_cast<int>(nodePos.y), static_cast<int>(map.tileSizeX), static_cast<int>(map.tileSizeY), BLUE);

                // Update current node to the previous node in the path
                currentNode = pathfinder.cheapestEdgeTo[currentNode];
            }
        }

        DrawRectangle( 10, 10, 310, 20, BLACK);
        DrawText("PRESS ~ TO OPEN THE MENU :)", 10, 10, 20, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}