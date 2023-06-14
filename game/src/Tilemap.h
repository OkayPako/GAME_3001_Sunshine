#pragma once
#include "raylib.h"
#include "TileCoord.h"

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
    bool isTranversable[(int)Tile::count];

public:
    float tileSizeX = 64; // width of a tile in pixels
    float tileSizeY = 64; // height of a tile in pixels

    TileCoord playerPosition;  // New player position variable

    Color tileColors[(int)Tile::count];

    Tile tiles[MAP_WIDTH][MAP_HEIGHT];

    Tilemap()
    {
        tileColors[(int)Tile::floor] = BEIGE;
        tileColors[(int)Tile::grass] = GREEN;
        tileColors[(int)Tile::water] = SKYBLUE;
        tileColors[(int)Tile::wall] = DARKGRAY;

        isTranversable[(int)Tile::floor] = true;
        isTranversable[(int)Tile::grass] = true;
        isTranversable[(int)Tile::water] = false;
        isTranversable[(int)Tile::wall] = false;
    }

    Vector2 GetScreenPositionOfTile(TileCoord coordinate)
    {
        return {coordinate.x * tileSizeX, coordinate.y * tileSizeY};
    }

    // The function calculates the tile coordinate based on the screen coordinate by dividing the x and y values by the tile size and converting them to integers.
    // It returns the calculated tile coordinate as a Vector2 object.

    Vector2 GetTileAtScreenPosition(Vector2 coordinate)
    {
        return { coordinate.x / tileSizeX, coordinate.y / tileSizeY };
    }

    // The function first checks if the given coordinate is within the bounds of the tilemap using conditional statements.
    // If the coordinate is valid, it retrieves the tile type at that coordinate from the tiles array.
    // It then uses the isTranversable array to determine if the tile is traversable by converting the tile type to an integer and accessing the corresponding element in the array.
    // If the coordinate is out of bounds, it returns false to indicate that the tile is not traversable.

    bool isTileTraversable(TileCoord coordinate)
    {
        if (coordinate.x >= 0 && coordinate.x < MAP_WIDTH && coordinate.y >= 0 && coordinate.y < MAP_HEIGHT)
        {
            Tile tileType = tiles[coordinate.x][coordinate.y];
            return isTranversable[static_cast<int>(tileType)];
        }

        return false; // Return false for out-of-bounds coordinates
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
                else if(rand() % 90 + 1 < chanceOfFloor)
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
            } 
            while (tiles[playerX][playerY] != Tile::floor);

            playerPosition = TileCoord(playerX, playerY);
        }
    }

    void Draw()
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                Tile tileType = tiles[x][y];
                Color tileColor = tileColors[(int)tileType];

                DrawRectangle(x * tileSizeX, y * tileSizeY, tileSizeX, tileSizeY, tileColor);


                // Draw the player sprite with the current frame
                Vector2 playerScreenPosition = GetScreenPositionOfTile(playerPosition);
                DrawRectangle(static_cast<int>(playerScreenPosition.x), static_cast<int>(playerScreenPosition.y), tileSizeX, tileSizeY, RED);
            }
        }

    }
};
