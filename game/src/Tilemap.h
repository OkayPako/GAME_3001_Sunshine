#pragma once
#include "raylib.h"
#include "TileCoord.h"
#include <vector>

#define MAP_WIDTH 20
#define MAP_HEIGHT 12

enum class Tile
{
    floor = 0,
    grass,
    water,
    wall,
    player, // New player tile type :)
    count
};

class Tilemap
{
private:
    bool isTraversable[(int)Tile::count];
    Texture2D tileTextures[(int)Tile::count];
    bool useTextures;
    bool showInfo;
    bool showGPS;

public:
    float tileSizeX = 64; // width of a tile in pixels
    float tileSizeY = 64; // height of a tile in pixels

    TileCoord playerPosition;
    Texture2D playerTexture;
    Rectangle playerRect;
    int frameWidth = 64;
    int frameHeight = 64;
    float frameTime;
    float elapsedTime;

    Color tileColors[(int)Tile::count];

    Tile tiles[MAP_WIDTH][MAP_HEIGHT];

    Tilemap(Texture2D playerTexture)
    {
        this->playerTexture = playerTexture;

        Image floorImage = LoadImage("../game/assets/textures/floor.png");
        Image grassImage = LoadImage("../game/assets/textures/grass.png");
        Image waterImage = LoadImage("../game/assets/textures/water.png");
        Image wallImage = LoadImage("../game/assets/textures/wall.png");

        // Resize the tile textures
        ImageResizeNN(&floorImage, tileSizeX,tileSizeY);
        ImageResizeNN(&grassImage, tileSizeX,tileSizeY);
        ImageResizeNN(&waterImage, tileSizeX,tileSizeY);
        ImageResizeNN(&wallImage, tileSizeX, tileSizeY);

        tileTextures[(int)Tile::floor] = LoadTextureFromImage(floorImage);
        tileTextures[(int)Tile::grass] = LoadTextureFromImage(grassImage);
        tileTextures[(int)Tile::water] = LoadTextureFromImage(waterImage);
        tileTextures[(int)Tile::wall] = LoadTextureFromImage(wallImage);

        tileColors[(int)Tile::floor] = BEIGE;
        tileColors[(int)Tile::grass] = GREEN;
        tileColors[(int)Tile::water] = SKYBLUE;
        tileColors[(int)Tile::wall] = DARKGRAY;

        isTraversable[(int)Tile::floor] = true;
        isTraversable[(int)Tile::grass] = true;
        isTraversable[(int)Tile::water] = false;
        isTraversable[(int)Tile::wall] = false;

        frameWidth = playerTexture.width / 4;
        frameHeight = playerTexture.height;
        frameTime = 0.2f;
        elapsedTime = 0.0f;

        // Initialize the player rectangle at position (0, 0) with the frame size
        playerRect = { 0, 0, static_cast<float>(frameWidth), static_cast<float>(frameHeight) };

        useTextures = false;
        showInfo = false;
        showGPS = false;
    }

    Vector2 GetScreenPositionOfTile(TileCoord coordinate)
    {
        return { coordinate.x * tileSizeX, coordinate.y * tileSizeY };
    }

    Vector2 GetTileAtScreenPosition(Vector2 coordinate)
    {
        return { coordinate.x / tileSizeX, coordinate.y / tileSizeY };
    }

    bool IsTileTraversable(TileCoord coordinate)
    {
        if (coordinate.x >= 0 && coordinate.x < MAP_WIDTH && coordinate.y >= 0 && coordinate.y < MAP_HEIGHT)
        {
            Tile tileType = tiles[coordinate.x][coordinate.y];
            return isTraversable[static_cast<int>(tileType)];
        }

        return false; // Return false for out-of-bounds coordinates
    }

    void DrawBorders()
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            DrawLine(x * tileSizeX, 0, x * tileSizeX, MAP_HEIGHT * tileSizeY, BLACK);
        }
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            DrawLine(0, y * tileSizeY, MAP_WIDTH * tileSizeX, y * tileSizeY, BLACK);
        }
    }

    std::vector<Tile> GetAllTiles()
    {
        std::vector<Tile> allTiles;
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                Tile tileType = tiles[x][y];
                allTiles.push_back(tileType);
            }
        }
        return allTiles;
    }

    std::vector<TileCoord> GetConnectedTraversableTiles(TileCoord coordinate)
    {
        std::vector<TileCoord> connectedTiles;
        int dx[4] = { 1, -1, 0, 0 };
        int dy[4] = { 0, 0, 1, -1 };

        if (IsTileTraversable(coordinate))
        {
            connectedTiles.push_back(coordinate);

            for (int i = 0; i < 4; i++)
            {
                int nx = coordinate.x + dx[i];
                int ny = coordinate.y + dy[i];

                TileCoord adjacentTile(nx, ny);
                if (IsTileTraversable(adjacentTile))
                {
                    connectedTiles.push_back(adjacentTile);
                }
            }
        }

        return connectedTiles;
    }

    int GetTileTraversalCost(TileCoord coordinate)
    {
        Tile tileType = tiles[coordinate.x][coordinate.y];

        // Define traversal costs for different tile types
        switch (tileType)
        {
        case Tile::floor:
            return 1;
        case Tile::grass:
            return 2;
        case Tile::water:
            return -1; // Impassable tile
        case Tile::wall:
            return -1; // Impassable tile
        default:
            return 1;
        }
    }

    // Draw connections between traversable tiles

    // checks the traversability of the adjacent tiles individually in each cardinal direction (north, south, east, and west). 
    // If an adjacent tile in any of these directions is traversable, it calculates the center position of the adjacent tile 
    // and draws a line from the center of the current tile to the center of the adjacent tile
    void DrawGPS(Tilemap map)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                if (map.IsTileTraversable(TileCoord(x, y)))
                {
                    Vector2 tileCenter = map.GetScreenPositionOfTile(TileCoord(x, y)) + Vector2{ map.tileSizeX / 2, map.tileSizeY / 2 };

                    int adjacentX, adjacentY;

                    // Check North
                    adjacentX = x;
                    adjacentY = y - 1;
                    if (adjacentY >= 0 && map.IsTileTraversable(TileCoord(adjacentX, adjacentY)))
                    {
                        Vector2 adjacentTileCenter = map.GetScreenPositionOfTile(TileCoord(adjacentX, adjacentY)) + Vector2{ map.tileSizeX / 2, map.tileSizeY / 2 };
                        DrawLineEx(tileCenter, adjacentTileCenter, 2, DARKGREEN);
                    }

                    // Check South
                    adjacentX = x;
                    adjacentY = y + 1;
                    if (adjacentY < MAP_HEIGHT && map.IsTileTraversable(TileCoord(adjacentX, adjacentY)))
                    {
                        Vector2 adjacentTileCenter = map.GetScreenPositionOfTile(TileCoord(adjacentX, adjacentY)) + Vector2{ map.tileSizeX / 2, map.tileSizeY / 2 };
                        DrawLineEx(tileCenter, adjacentTileCenter, 2, DARKGREEN);
                    }

                    // Check East
                    adjacentX = x + 1;
                    adjacentY = y;
                    if (adjacentX < MAP_WIDTH && map.IsTileTraversable(TileCoord(adjacentX, adjacentY)))
                    {
                        Vector2 adjacentTileCenter = map.GetScreenPositionOfTile(TileCoord(adjacentX, adjacentY)) + Vector2{ map.tileSizeX / 2, map.tileSizeY / 2 };
                        DrawLineEx(tileCenter, adjacentTileCenter, 2, DARKGREEN);
                    }

                    // Check West
                    adjacentX = x - 1;
                    adjacentY = y;
                    if (adjacentX >= 0 && map.IsTileTraversable(TileCoord(adjacentX, adjacentY)))
                    {
                        Vector2 adjacentTileCenter = map.GetScreenPositionOfTile(TileCoord(adjacentX, adjacentY)) + Vector2{ map.tileSizeX / 2, map.tileSizeY / 2 };
                        DrawLineEx(tileCenter, adjacentTileCenter, 2, DARKGREEN);
                    }

                    DrawCircle(static_cast<int>(tileCenter.x), static_cast<int>(tileCenter.y), 8, DARKGREEN);
                }
            }
        }
    }

    void DrawInformationOnTiles()
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                Tile tileType = tiles[x][y];

                // Draw additional information on the tile
                switch (tileType)
                {
                case Tile::floor:
                    DrawText("Floor", x * tileSizeX + 10, y * tileSizeY + 10, 10, RAYWHITE);
                    break;
                case Tile::grass:
                    DrawText("Grass", x * tileSizeX + 10, y * tileSizeY + 10, 10, RAYWHITE);
                    break;
                case Tile::water:
                    DrawText("Water", x * tileSizeX + 10, y * tileSizeY + 10, 10, RAYWHITE);
                    break;
                case Tile::wall:
                    DrawText("Wall", x * tileSizeX + 10, y * tileSizeY + 10, 10, RAYWHITE);
                    break;
                default:
                    break;
                }
            }
        }
    }

    void Randomize(int chanceOfFloor = 50)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                if (rand() % 100 + 1 < chanceOfFloor)
                {
                    tiles[x][y] = Tile::floor;
                }
                else if (rand() % 90 + 1 < chanceOfFloor)
                {
                    tiles[x][y] = Tile::water;
                }
                else if (rand() % 80 + 1 < chanceOfFloor)
                {
                    tiles[x][y] = Tile::grass;
                }
                else
                {
                    tiles[x][y] = Tile::wall;
                }
            }

            // Randomly set player position on a floor tile
            int playerX, playerY;

            do
            {
                playerX = rand() % MAP_WIDTH;
                playerY = rand() % MAP_HEIGHT;
            } while (tiles[playerX][playerY] != Tile::floor);

            playerPosition = TileCoord(playerX, playerY);
        }
    }

    void Update(float deltaTime)
    {
        elapsedTime += deltaTime;
        if (elapsedTime >= frameTime)
        {
            playerRect.x += frameWidth;
            if (playerRect.x >= playerTexture.width)
            {
                playerRect.x = 0;
            }

            elapsedTime = 0.0f;
        }
    }

    void Draw()
    {
        if (useTextures)
        {
            DrawTextures();
        }
        else
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                for (int y = 0; y < MAP_HEIGHT; y++)
                {
                    Tile tileType = tiles[x][y];
                    Color tileColor = tileColors[static_cast<int>(tileType)];
                    DrawRectangle(x * tileSizeX, y * tileSizeY, tileSizeX, tileSizeY, tileColor);
                }
            }
        }
    }

    void DrawTextures()
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                Tile tileType = tiles[x][y];
                DrawTexture(tileTextures[static_cast<int>(tileType)], x * tileSizeX, y * tileSizeY, WHITE);
            }
        }
    }

    void SetUseTextures(bool useTextures)
    {
        this->useTextures = useTextures;
    }

    void SetShowInfo(bool showInfo)
    {
        this->showInfo = showInfo;
    }

    void DrawPlayer()
    {
        Vector2 playerScreenPosition = GetScreenPositionOfTile(playerPosition);
        Rectangle destRect = { playerScreenPosition.x, playerScreenPosition.y, frameWidth, frameHeight };
        DrawTextureRec(playerTexture, playerRect, playerScreenPosition, WHITE);
    }
};
