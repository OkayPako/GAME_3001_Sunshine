#pragma once
#include "raylib.h"
#include "TileCoord.h"

#define MAP_WIDTH 40
#define MAP_HEIGHT 23

enum class Tile
{
    floor = 0,
    grass,
    water,
    wall,
    count
};


class Tilemap
{
private:
    bool isTranversable[(int)Tile::count];

public:
    float tileSizeX = 32; // width of a tile in pixels
    float tileSizeY = 32; // height of a tile in pixels

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

    Vector2 GetTileAtScreenPos(Vector2 coordinate)
    {

    }

    bool isTileTraversable(TileCoord coordinate)
    {
        Tile tileType = tiles[coordinate.x][coordinate.y];
        return isTranversable[(int)Tile::count];
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
        }
    }

    void Draw()
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                // Vector2 coordinate = GetScreenPositionOfTile({x,y});
                // DrawRectangle(coordinate, {tileSizeX, tileSizeY});

                Tile tileType = tiles[x][y];
                Color tileColor = tileColors[(int)tileType];

                DrawRectangle(x * tileSizeX, y * tileSizeY, tileSizeX, tileSizeY, tileColor);
            }
        }
    }
};
