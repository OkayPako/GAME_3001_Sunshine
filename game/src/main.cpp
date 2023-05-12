#include "rlImGui.h"
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

class Circle
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

Circle circle;
Player player;

void changeColor(Circle& circle, Player& player, Color& lineColor1)
{
    // Collision Detection
    if (CheckCollisionCircles(Vector2{ circle.x, circle.y }, circle.circleRadius, Vector2{ player.x , player.y }, player.circleRadius))
    {
        lineColor1 = RED;
        circle.circleColor1 = RED;
        circle.circleColor2 = BLACK;
        player.circleColor1 = RED;
        player.circleColor2 = BLACK;
    }
    else
    {
        lineColor1 = RAYWHITE;
        circle.circleColor1 = RAYWHITE;
        circle.circleColor2 = BLACK;
        player.circleColor1 = LIGHTGRAY;
        player.circleColor2 = BLACK;
    }
}

int main(void)
{
    // Window/Audio initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    InitAudioDevice();
    SetTargetFPS(60);

    // Loading of Textures/Music
    Texture2D ive = LoadTexture("../game/assets/textures/ive.png");
    Music music = LoadMusicStream("../game/assets/audio/I AM.mp3");
    float timePlayed;

    // Looping/Playing/Volume/Time played and pause of music
    music.looping = true;
    PlayMusicStream(music);
    float volume = 0.1f;
    bool pause = false;

    // Line Color
    Color lineColor1 = RAYWHITE;
    Color lineColor2 = BLACK;

    // Stationary Circle Traits
    circle.x = SCREEN_WIDTH / 2;
    circle.y = SCREEN_HEIGHT / 2;
    circle.circleColor1 = RAYWHITE;
    circle.circleColor2 = BLACK;

    // Player Circle Traits
    player.circleColor1 = LIGHTGRAY;
    player.circleColor2 = BLACK;

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        BeginDrawing();

        ClearBackground(BLACK);

        // Music Volume and play/pause
        if (IsKeyDown(KEY_SPACE))
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

        // Non-colliding text and shapes
        DrawRectangleGradientV(0, 1030, 1920, 50, lineColor1, lineColor2);
        DrawRectangleGradientV(0, 10, 1920, 50, lineColor2, lineColor1);
        DrawText("Stream I AM by IVE!", 840, 20, 25, RAYWHITE);

        // Get timePlayed scaled to bar dimensions
        timePlayed = GetMusicTimePlayed(music) / GetMusicTimeLength(music) * (SCREEN_WIDTH - 40);
        DrawRectangle(20, SCREEN_HEIGHT - 100 - 12, SCREEN_WIDTH - 40, 12, LIGHTGRAY);
        DrawRectangle(20, SCREEN_HEIGHT - 100 - 12, (int)timePlayed, 12, MAROON);
        DrawRectangleLines(20, SCREEN_HEIGHT - 100 - 12, SCREEN_WIDTH - 40, 12, GRAY);

        // Non-moving Circle
        circle.Draw();

        // Player Circle
        player.x = GetMouseX();
        player.y = GetMouseY();
        changeColor(circle, player, lineColor1);
        player.Draw();

        EndDrawing();
    }

    UnloadMusicStream(music);

    CloseAudioDevice();

    CloseWindow();
    return 0;
}
