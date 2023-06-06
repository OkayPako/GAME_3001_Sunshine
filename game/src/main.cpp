#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Defines the properties of the seeker, including position, velocity, acceleration, direction, and angular speed.
struct Rigidbody
{
    Vector2 pos{};
    Vector2 vel{};
    Vector2 acc{};

    Vector2 dir{};
    float angularSpeed;
};

// Checks if a line segment intersects with a circle, which is used later to detect collisions between the seeker and an obstacle.
bool CheckCollisionLineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius)
{
    Vector2 nearest = NearestPoint(lineStart, lineEnd, circlePosition);
    return DistanceSqr(nearest, circlePosition) <= circleRadius * circleRadius;
}

// Updates the seeker's position and velocity based on its acceleration and the time step dt. 

// It uses basic kinematic equations to calculate the new values.
// velocity = velocity * acceleration * deltaTime
// position(2) = position(1) * acceleration + velocity * deltaTime + 0.5 * acceleration * deltaTime * deltaTime

void Integrate(Rigidbody& rb, float dt)
{
    rb.vel = rb.vel + rb.acc * dt;
    rb.pos = rb.pos + rb.vel * dt + rb.acc * dt * dt * 0.5f;
    rb.dir = RotateTowards(rb.dir, Normalize(rb.vel), rb.angularSpeed * dt);
}

// Calculates a steering force that directs the seeker towards a target position while limiting its maximum speed.
Vector2 Seek(const Vector2& pos, const Rigidbody& rb, float maxSpeed)
{
    return Normalize(pos - rb.pos) * maxSpeed - rb.vel;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    Rigidbody seeker;
    seeker.pos = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    seeker.dir = { 0.0f, 1.0f };
    seeker.angularSpeed = 100.0f;
    float seekerLineLength = 100.0f;
    float seekerRadius = 25.0f;

    Vector2 obstaclePosition{ SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.25f };
    float obstacleRadius = 50.0f;

    bool useGUI = false;
    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();

        Vector2 right = Rotate(seeker.dir, 30.0f * DEG2RAD);
        Vector2 left = Rotate(seeker.dir, -30.0f * DEG2RAD);
        Vector2 rightEnd = seeker.pos + right * seekerLineLength;
        Vector2 leftEnd = seeker.pos + left * seekerLineLength;

        // Determines the desired steering force towards the current mouse position.
        seeker.acc = Seek(GetMousePosition(), seeker, 100.0f);

        // Function is called to update the seeker's position and velocity.
        Integrate(seeker, dt);

        // Checks for collisions between the seeker's probe lines and the obstacle
        bool leftCollision = CheckCollisionLineCircle(seeker.pos, leftEnd, obstaclePosition, obstacleRadius);
        bool rightCollision = CheckCollisionLineCircle(seeker.pos, rightEnd, obstaclePosition, obstacleRadius);

        // If a collision occurs, the seeker's velocity is adjusted to rotate away from the obstacle, simulating avoidance behavior.
        if (rightCollision)
        {
            Vector2 linearDirection = Normalize(seeker.vel);
            float linearSpeed = Length(seeker.vel);
            seeker.vel = Rotate(linearDirection, -seeker.angularSpeed * dt * DEG2RAD) * linearSpeed;
        }

        if (leftCollision)
        {
            Vector2 linearDirection = Normalize(seeker.vel);
            float linearSpeed = Length(seeker.vel);
            seeker.vel = Rotate(linearDirection, seeker.angularSpeed * dt * DEG2RAD) * linearSpeed;
        }

        // Colors of the probe lines are determined based on whether a collision occurred, with red indicating a collision and green indicating no collision.
        Color rightColor = rightCollision ? RED : GREEN;
        Color leftColor = leftCollision ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Drawing agent and obstacle
        DrawCircleV(seeker.pos, seekerRadius, BLUE);
        DrawCircleV(obstaclePosition, obstacleRadius, GRAY);

        // Whiskers
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