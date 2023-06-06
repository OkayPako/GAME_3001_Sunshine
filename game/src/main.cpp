#include "rlImGui.h"
#include "Math.h"
#include <vector>
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

// Updates the agent's position and velocity based on its acceleration and the time step dt. 
// It uses basic kinematic equations to calculate the new values.
// velocity = velocity * acceleration * deltaTime
// position(2) = position(1) * acceleration + velocity * deltaTime + 0.5 * acceleration * deltaTime * deltaTime
void Integrate(Rigidbody& rb, float dt)
{
    rb.vel = rb.vel + rb.acc * dt;
    rb.pos = rb.pos + rb.vel * dt + rb.acc * dt * dt * 0.5f;
    rb.dir = RotateTowards(rb.dir, Normalize(rb.vel), rb.angularSpeed * dt);
}

// Calculates a steering force that directs the agent towards a target position while limiting its maximum speed.
Vector2 Seek(const Vector2& pos, const Rigidbody& rb, float maxSpeed)
{
    return Normalize(pos - rb.pos) * maxSpeed - rb.vel;
}

// Agent class definition
class Agent
{
public:
    Rigidbody rb;
    float lineLength;
    float radius;

    Agent(float startX, float startY, float angularSpeed, float lineLength, float radius)
        : lineLength(lineLength), radius(radius)
    {
        rb.pos = { startX, startY };
        rb.dir = { 0.0f, 1.0f };
        rb.angularSpeed = angularSpeed;
    }

    // Checks for collisions with obstacle and performs obstacle avoidance
    void ObstacleAvoidance(const Vector2& obstaclePosition, float obstacleRadius, float avoidanceForce, float dt)
    {
        Vector2 right = Rotate(rb.dir, 30.0f * DEG2RAD);
        Vector2 left = Rotate(rb.dir, -30.0f * DEG2RAD);
        Vector2 rightEnd = rb.pos + right * lineLength;
        Vector2 leftEnd = rb.pos + left * lineLength;

        bool leftCollision = CheckCollisionLineCircle(rb.pos, leftEnd, obstaclePosition, obstacleRadius);
        bool rightCollision = CheckCollisionLineCircle(rb.pos, rightEnd, obstaclePosition, obstacleRadius);

        if (rightCollision)
        {
            Vector2 linearDirection = Normalize(rb.vel);
            float linearSpeed = Length(rb.vel);
            rb.vel = Rotate(linearDirection, -rb.angularSpeed * dt * DEG2RAD) * linearSpeed;
        }

        if (leftCollision)
        {
            Vector2 linearDirection = Normalize(rb.vel);
            float linearSpeed = Length(rb.vel);
            rb.vel = Rotate(linearDirection, rb.angularSpeed * dt * DEG2RAD) * linearSpeed;
        }

        // Apply avoidance force
        if (leftCollision || rightCollision)
        {
            Vector2 avoidanceDir = Normalize(rb.pos - obstaclePosition);
            rb.acc = rb.acc + avoidanceDir * avoidanceForce;
        }
    }
};

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    Agent agent(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 100.0f, 100.0f, 25.0f);

    std::vector<Vector2> obstaclePositions;  // Store the positions of obstacles placed by the mouse

    bool useGUI = false;
    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();

        agent.rb.acc = Seek(GetMousePosition(), agent.rb, 100.0f);

        Integrate(agent.rb, dt);

        // Loop through all obstacle positions and perform obstacle avoidance
        for (const auto& obstaclePosition : obstaclePositions)
        {
            agent.ObstacleAvoidance(obstaclePosition, 50.0f, 100.0f, dt);
        }

        // Add obstacle on mouse click
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            obstaclePositions.push_back(GetMousePosition());
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Drawing agent and obstacles
        DrawCircleV(agent.rb.pos, agent.radius, BLUE);
        for (const auto& obstaclePosition : obstaclePositions)
        {
            DrawCircleV(obstaclePosition, 50.0f, GRAY);
        }

        // Whiskers
        Vector2 right = Rotate(agent.rb.dir, 30.0f * DEG2RAD);
        Vector2 left = Rotate(agent.rb.dir, -30.0f * DEG2RAD);
        Vector2 rightEnd = agent.rb.pos + right * agent.lineLength;
        Vector2 leftEnd = agent.rb.pos + left * agent.lineLength;
        bool leftCollision = false;
        bool rightCollision = false;

        // Check collisions with each obstacle position individually
        for (const auto& obstaclePosition : obstaclePositions)
        {
            leftCollision |= CheckCollisionLineCircle(agent.rb.pos, leftEnd, obstaclePosition, 50.0f);
            rightCollision |= CheckCollisionLineCircle(agent.rb.pos, rightEnd, obstaclePosition, 50.0f);
        }

        Color rightColor = rightCollision ? RED : GREEN;
        Color leftColor = leftCollision ? RED : GREEN;
        DrawLineV(agent.rb.pos, rightEnd, rightColor);
        DrawLineV(agent.rb.pos, leftEnd, leftColor);

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