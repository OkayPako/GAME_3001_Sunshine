#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

bool CheckCollisionLineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius)
{
    Vector2 nearest = NearestPoint(lineStart, lineEnd, circlePosition);
    return DistanceSqr(nearest, circlePosition) <= circleRadius * circleRadius;
}

struct Rigidbody
{
    Vector2 pos{};
    Vector2 vel{};
    Vector2 acc{};

    Vector2 dir{};
    float angularSpeed;
};

void Integrate(Rigidbody& rb, float dt)
{
    rb.vel = rb.vel + rb.acc * dt;
    rb.pos = rb.pos + rb.vel * dt + rb.acc * dt * dt * 0.5f;
    rb.dir = RotateTowards(rb.dir, Normalize(rb.vel), rb.angularSpeed * dt);
}

Vector2 Seek(const Vector2& pos, const Rigidbody& rb, float maxSpeed)
{
    return Normalize(pos - rb.pos) * maxSpeed - rb.vel;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    float seekerProbeLength = 100.0f;
    float seekerRadius = 25.0f;
    Rigidbody seeker;
    seeker.pos = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    seeker.dir = { 0.0f, 1.0f };
    seeker.angularSpeed = 100.0f;

    Vector2 obstaclePosition{ SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.25f };
    float obstacleRadius = 50.0f;

    bool useGUI = false;
    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();

        Vector2 right = Rotate(seeker.dir, 30.0f * DEG2RAD);
        Vector2 left = Rotate(seeker.dir, -30.0f * DEG2RAD);
        Vector2 rightEnd = seeker.pos + right * seekerProbeLength;
        Vector2 leftEnd = seeker.pos + left * seekerProbeLength;

        seeker.acc = Seek(GetMousePosition(), seeker, 100.0f);
        Integrate(seeker, dt);

        bool leftCollision = CheckCollisionLineCircle(seeker.pos, leftEnd, obstaclePosition, obstacleRadius);
        bool rightCollision = CheckCollisionLineCircle(seeker.pos, rightEnd, obstaclePosition, obstacleRadius);
        if (rightCollision)
        {
            Vector2 linearDirection = Normalize(seeker.vel);
            float linearSpeed = Length(seeker.vel);
            seeker.vel = Rotate(linearDirection, -seeker.angularSpeed * dt * DEG2RAD) * linearSpeed;
        }
        Color rightColor = rightCollision ? RED : GREEN;
        Color leftColor = leftCollision ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(seeker.pos, seekerRadius, BLUE);
        DrawCircleV(obstaclePosition, obstacleRadius, GRAY);
        DrawLineV(seeker.pos, rightEnd, rightColor);
        DrawLineV(seeker.pos, leftEnd, leftColor);

        if (IsKeyPressed(KEY_GRAVE)) useGUI = !useGUI;
        if (useGUI)
        {
            rlImGuiBegin();
            rlImGuiEnd();
        }

        DrawFPS(10, 10);
        DrawText("Press ~ to open/close GUI", 10, 30, 20, GRAY);
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}