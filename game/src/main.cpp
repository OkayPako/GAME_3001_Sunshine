// Raphael Flores 101139370

#include "rlImGui.h"
#include <vector>
#include <ctime>
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

class Circle
{
public:
    float x, y;
    float circleRadius = 40;
    Color circleColor1;
    Color circleColor2;
    int deleteTimer = 0;

    void Draw()
    {
        DrawCircleGradient(x, y, circleRadius, circleColor1, circleColor2);
    }
};

class Player
{
public:
    float x, y;
    float circleRadius = 40;
    Color circleColor1;
    Color circleColor2;

    void Draw()
    {
        DrawCircleGradient(x, y, circleRadius, circleColor1, circleColor2);
    }
};

Circle stationaryCircle;
std::vector<Circle> circles;
Player player;
int score = 0;
int circlesSpawned = 0;
int circlesMissed = 0;
float deltaTime = 1 / 60;
float circleSpawnTimer = 0.5082f;

void UI()
{
    DrawText("Stream I AM by IVE!", 840, 20, 25, RAYWHITE);
    DrawText(TextFormat("Score: %i", score), 10, 70, 40, LIGHTGRAY);
    DrawText(TextFormat("Beats Spawned: %i", circlesSpawned), 1100, 70, 40, LIGHTGRAY);
    DrawText(TextFormat("Beats Missed: %i", circlesMissed), 1550, 70, 40, LIGHTGRAY);
}

void input()
{
    for (auto it = circles.begin(); it != circles.end();)
    {
        Circle& c = *it;
        c.deleteTimer += 1;
        if (c.deleteTimer >= 90 && !CheckCollisionCircles(Vector2{ c.x, c.y }, c.circleRadius, Vector2{ player.x , player.y }, player.circleRadius) && !IsKeyPressed(KEY_Z))
        {
            it = circles.erase(it);
            circlesMissed += 1;
            score -= 100;
        }
        else if (c.deleteTimer < 90 && CheckCollisionCircles(Vector2{ c.x, c.y }, c.circleRadius, Vector2{ player.x , player.y }, player.circleRadius) && IsKeyPressed(KEY_Z))
        {
            it = circles.erase(it);
            score += 100;
        }
        else
        {
            it++;
        }
        c.Draw();
    }
}

void spawnCircles(std::vector<Circle>& circles)
{
    Circle circle;
    circle.circleRadius = 40;
    int maxX = SCREEN_WIDTH - (circle.circleRadius * 2);
    int maxY = (SCREEN_HEIGHT - (circle.circleRadius * 2) - 200);
    circle.x = rand() % maxX + circle.circleRadius;
    circle.y = rand() % maxY + (circle.circleRadius * 4);
    circle.circleColor1 = BLUE;
    circle.circleColor2 = BLACK;
    circles.push_back(circle);
}

void changeColor(Player& player, Color& lineColor1, std::vector<Circle>& circles)
{
    bool collision = false;
    for (auto& c : circles)
    {
        // Collision Detection (Center of circle 1, Radius of Circle 1, Center of circle 2, Radius of circle 2)
        if (CheckCollisionCircles(Vector2{ c.x, c.y }, c.circleRadius, Vector2{ player.x , player.y }, player.circleRadius))
        {
            lineColor1 = RED;
            c.circleColor1 = RED;
            c.circleColor2 = BLACK;
            player.circleColor1 = RED;
            player.circleColor2 = BLACK;
            collision = true;
        }
        else
        {
            c.circleColor1 = VIOLET;
            c.circleColor2 = BLACK;
        }
    }

    if (!collision)
    {
        lineColor1 = RAYWHITE;
        player.circleColor1 = BLANK;
        player.circleColor2 = RAYWHITE;
    }
}

int main(void)
{
    // Window/Audio initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ive-SU(?)");
    SetTargetFPS(60);
    HideCursor();
    InitAudioDevice();
    srand(time(NULL));

    // Loading of Textures/Music
    Texture2D ive = LoadTexture("../game/assets/textures/ive.png");
    Music music = LoadMusicStream("../game/assets/audio/I AM.mp3");
    float timePlayed;

    // Looping/Playing/Volume/Time played and pause of music
    music.looping = true;
    PlayMusicStream(music);
    float volume = 0.2f;
    bool pause = false;

    // Line Color
    Color lineColor1 = RAYWHITE;
    Color lineColor2 = BLACK;

    // Player Circle Traits
    player.circleColor1 = BLANK;
    player.circleColor2 = RAYWHITE;

    while (!WindowShouldClose())
    {
        deltaTime = GetFrameTime();

        UpdateMusicStream(music);

        BeginDrawing();

        ClearBackground(BLACK);

        // Music Volume and play/pause
        if (IsKeyPressed(KEY_SPACE))
        {
            pause = !pause;

            if (pause) PauseMusicStream(music);
            else ResumeMusicStream(music);
        }

        if (IsKeyDown(KEY_DOWN)) volume -= 0.01f;
        else if (IsKeyDown(KEY_UP)) volume += 0.01f;
        SetMusicVolume(music, volume);

        // Background Texture
        DrawTexture(ive, 0, 0, RAYWHITE);

        UI();

        input();

        // Non-colliding text and shapes
        DrawRectangleGradientV(0, 1030, 1920, 50, lineColor1, lineColor2);
        DrawRectangleGradientV(0, 10, 1920, 50, lineColor2, lineColor1);

        // Get timePlayed scaled to bar dimensions
        timePlayed = GetMusicTimePlayed(music) / GetMusicTimeLength(music) * (SCREEN_WIDTH - 40);
        DrawRectangle(20, SCREEN_HEIGHT - 100 - 12, SCREEN_WIDTH - 40, 12, LIGHTGRAY);
        DrawRectangle(20, SCREEN_HEIGHT - 100 - 12, (int)timePlayed, 12, MAROON);
        DrawRectangleLines(20, SCREEN_HEIGHT - 100 - 12, SCREEN_WIDTH - 40, 12, GRAY);

        // Player Circle
        player.x = GetMouseX();
        player.y = GetMouseY() + (player.circleRadius/ 2);
        player.Draw();
        circleSpawnTimer -= deltaTime;

        // Enemy Circle
        if (circleSpawnTimer <= 0)
        {
            spawnCircles(circles);
            circlesSpawned += 1;
            circleSpawnTimer = 0.5082f;
        }

        changeColor(player, lineColor1, circles);

        EndDrawing();
    }

    UnloadMusicStream(music);

    CloseAudioDevice();

    CloseWindow();
    return 0;
}
