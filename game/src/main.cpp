#include "rlImGui.h"
#include "Tilemap.h"
#include "raylib.h"
#include <time.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 768

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

        // Player movement
        if (IsKeyPressed(KEY_W) && map.isTileTraversable(map.playerPosition - TileCoord(0, 1)))
            map.playerPosition -= TileCoord(0, 1);
        if (IsKeyPressed(KEY_S) && map.isTileTraversable(map.playerPosition + TileCoord(0, 1)))
            map.playerPosition += TileCoord(0, 1);
        if (IsKeyPressed(KEY_A) && map.isTileTraversable(map.playerPosition - TileCoord(1, 0)))
            map.playerPosition -= TileCoord(1, 0);
        if (IsKeyPressed(KEY_D) && map.isTileTraversable(map.playerPosition + TileCoord(1, 0)))
            map.playerPosition += TileCoord(1, 0);

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

        // Draw connections between traversable tiles

        // checks the traversability of the adjacent tiles individually in each cardinal direction (north, south, east, and west). 
        // If an adjacent tile in any of these directions is traversable, it calculates the center position of the adjacent tile 
        // and draws a line from the center of the current tile to the center of the adjacent tile

        if (useGUI)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                for (int y = 0; y < MAP_HEIGHT; y++)
                {
                    if (map.isTileTraversable(TileCoord(x, y)))
                    {
                        Vector2 tileCenter = map.GetScreenPositionOfTile(TileCoord(x, y)) + Vector2{ map.tileSizeX / 2, map.tileSizeY / 2 };

                        int adjacentX, adjacentY;

                        // Check North
                        adjacentX = x;
                        adjacentY = y - 1;
                        if (adjacentY >= 0 && map.isTileTraversable(TileCoord(adjacentX, adjacentY)))
                        {
                            Vector2 adjacentTileCenter = map.GetScreenPositionOfTile(TileCoord(adjacentX, adjacentY)) + Vector2{ map.tileSizeX / 2, map.tileSizeY / 2 };
                            DrawLineEx(tileCenter, adjacentTileCenter, 2, DARKGREEN);
                        }

                        // Check South
                        adjacentX = x;
                        adjacentY = y + 1;
                        if (adjacentY < MAP_HEIGHT && map.isTileTraversable(TileCoord(adjacentX, adjacentY)))
                        {
                            Vector2 adjacentTileCenter = map.GetScreenPositionOfTile(TileCoord(adjacentX, adjacentY)) + Vector2{ map.tileSizeX / 2, map.tileSizeY / 2 };
                            DrawLineEx(tileCenter, adjacentTileCenter, 2, DARKGREEN);
                        }

                        // Check East
                        adjacentX = x + 1;
                        adjacentY = y;
                        if (adjacentX < MAP_WIDTH && map.isTileTraversable(TileCoord(adjacentX, adjacentY)))
                        {
                            Vector2 adjacentTileCenter = map.GetScreenPositionOfTile(TileCoord(adjacentX, adjacentY)) + Vector2{ map.tileSizeX / 2, map.tileSizeY / 2 };
                            DrawLineEx(tileCenter, adjacentTileCenter, 2, DARKGREEN);
                        }

                        // Check West
                        adjacentX = x - 1;
                        adjacentY = y;
                        if (adjacentX >= 0 && map.isTileTraversable(TileCoord(adjacentX, adjacentY)))
                        {
                            Vector2 adjacentTileCenter = map.GetScreenPositionOfTile(TileCoord(adjacentX, adjacentY)) + Vector2{ map.tileSizeX / 2, map.tileSizeY / 2 };
                            DrawLineEx(tileCenter, adjacentTileCenter, 2, DARKGREEN);
                        }

                        DrawCircle(static_cast<int>(tileCenter.x), static_cast<int>(tileCenter.y), 8, RED);
                    }
                }
            }
        }

        // Draw connections between traversable tiles that are on all axis

        //if (useGUI)
        //{
        //    for (int x = 0; x < MAP_WIDTH; x++)
        //    {
        //        for (int y = 0; y < MAP_HEIGHT; y++)
        //        {
        //            if (map.isTileTraversable(TileCoord(x, y)))
        //            {
        //                Vector2 tileCenter = map.GetScreenPositionOfTile(TileCoord(x, y)) + Vector2{ map.tileSizeX / 2, map.tileSizeY / 2 };
        //
        //                for (int dx = -1; dx <= 1; dx++)
        //                {
        //                    for (int dy = -1; dy <= 1; dy++)
        //                    {
        //                        if (dx == 0 && dy == 0)
        //                            continue;
        //
        //                        int adjacentX = x + dx;
        //                        int adjacentY = y + dy;
        //
        //                        if (adjacentX >= 0 && adjacentX < MAP_WIDTH && adjacentY >= 0 && adjacentY < MAP_HEIGHT && map.isTileTraversable(TileCoord(adjacentX, adjacentY)))
        //                        {
        //                            Vector2 adjacentTileCenter = map.GetScreenPositionOfTile(TileCoord(adjacentX, adjacentY)) + Vector2{ map.tileSizeX / 2, map.tileSizeY / 2 };
        //                            DrawLineEx(tileCenter, adjacentTileCenter, 2, DARKGREEN);
        //                        }
        //                    }
        //                }
        //                DrawCircle(static_cast<int>(tileCenter.x), static_cast<int>(tileCenter.y), 10, RED);
        //            }
        //        }
        //    }
        //}

        DrawText("Tile Map!", 20, 10, 20, RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}