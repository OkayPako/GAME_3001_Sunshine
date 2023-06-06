#include "rlImGui.h"
#include "Math.h"
#include <vector>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Defines the properties of the seeker, including position, velocity, acceleration, direction, and angular speed.
struct Rigidbody
{
    Vector2 pos{};          // Position
    Vector2 vel{};          // Velocity
    Vector2 acc{};          // Acceleration

    Vector2 dir{};          // Direction
    float angularSpeed;     // Angular speed (in degrees per second)
};

// Checks if a line segment intersects with a circle, which is used later to detect collisions between the seeker and an obstacle.
bool CheckCollisionLineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius)
{
    Vector2 nearest = NearestPoint(lineStart, lineEnd, circlePosition);
    return DistanceSqr(nearest, circlePosition) <= circleRadius * circleRadius;
}

// Updates the agent's position and velocity based on its acceleration and the time step dt. 
// It uses basic kinematic equations to calculate the new values.
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
    Rigidbody rb;           // Rigidbody of the agent
    float lineLength;       // Length of the whiskers
    float radius;           // Radius of the agent

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
        Vector2 right2 = Rotate(rb.dir, 60.0f * DEG2RAD);
        Vector2 left2 = Rotate(rb.dir, -60.0f * DEG2RAD);
        Vector2 rightEnd = rb.pos + right * lineLength;
        Vector2 leftEnd = rb.pos + left * lineLength;
        Vector2 right2End = rb.pos + right2 * lineLength;
        Vector2 left2End = rb.pos + left2 * lineLength;

        bool leftCollision = CheckCollisionLineCircle(rb.pos, leftEnd, obstaclePosition, obstacleRadius);
        bool rightCollision = CheckCollisionLineCircle(rb.pos, rightEnd, obstaclePosition, obstacleRadius);
        bool left2Collision = CheckCollisionLineCircle(rb.pos, left2End, obstaclePosition, obstacleRadius);
        bool right2Collision = CheckCollisionLineCircle(rb.pos, right2End, obstaclePosition, obstacleRadius);

        if (rightCollision || leftCollision || right2Collision || left2Collision)
        {
            // Rotate the agent's velocity in the opposite direction to simulate avoidance behavior
            Vector2 linearDirection = Normalize(rb.vel);
            float linearSpeed = Length(rb.vel);
            rb.vel = Rotate(linearDirection, -rb.angularSpeed * dt * DEG2RAD) * linearSpeed;
        }

        // Apply avoidance force
        if (rightCollision)
        {
            Vector2 avoidanceDir = Rotate(rb.dir, -30.0f * DEG2RAD);
            rb.acc = rb.acc + avoidanceDir * avoidanceForce;
        }

        if (leftCollision)
        {
            Vector2 avoidanceDir = Rotate(rb.dir, 30.0f * DEG2RAD);
            rb.acc = rb.acc + avoidanceDir * avoidanceForce;
        }

        if (right2Collision)
        {
            Vector2 avoidanceDir = Rotate(rb.dir, -60.0f * DEG2RAD);
            rb.acc = rb.acc + avoidanceDir * avoidanceForce;
        }

        if (left2Collision)
        {
            Vector2 avoidanceDir = Rotate(rb.dir, 60.0f * DEG2RAD);
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
        ClearBackground(BLACK);

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
        Vector2 right2 = Rotate(agent.rb.dir, 60.0f * DEG2RAD);
        Vector2 left2 = Rotate(agent.rb.dir, -60.0f * DEG2RAD);
        Vector2 right2End = agent.rb.pos + right2 * agent.lineLength;
        Vector2 left2End = agent.rb.pos + left2 * agent.lineLength;

        bool leftCollision = false;
        bool rightCollision = false;
        bool left2Collision = false;
        bool right2Collision = false;

        // Check collisions with each obstacle position individually
        for (const auto& obstaclePosition : obstaclePositions)
        {
            leftCollision |= CheckCollisionLineCircle(agent.rb.pos, leftEnd, obstaclePosition, 50.0f);
            rightCollision |= CheckCollisionLineCircle(agent.rb.pos, rightEnd, obstaclePosition, 50.0f);
            left2Collision |= CheckCollisionLineCircle(agent.rb.pos, left2End, obstaclePosition, 50.0f);
            right2Collision |= CheckCollisionLineCircle(agent.rb.pos, right2End, obstaclePosition, 50.0f);
        }

        Color rightColor = rightCollision ? RED : GREEN;
        Color leftColor = leftCollision ? RED : GREEN;
        Color right2Color = right2Collision ? RED : GREEN;
        Color left2Color = left2Collision ? RED : GREEN;

        DrawLineV(agent.rb.pos, rightEnd, rightColor);
        DrawLineV(agent.rb.pos, leftEnd, leftColor);
        DrawLineV(agent.rb.pos, right2End, right2Color);
        DrawLineV(agent.rb.pos, left2End, left2Color);

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

// In the main function, the code sets up the window and initializes the ImGui library for graphical user interface.
// It creates an Agent object with an initial position, angular speed, line length, and radius. It also declares a vector to store obstacle positions.

// Inside the game loop, the code calculates the agent's acceleration based on its current position and the position of the mouse cursor using the Seek function. 
// The Integrate function is then called to update the agent's position and velocity based on the acceleration and time step.

// Next, the code iterates over all obstacle positions and calls the ObstacleAvoidance function on the agent to check for collisions and perform obstacle avoidance behavior.
// If a collision is detected, the agent's velocity is rotated in the opposite direction to simulate avoidance. Additionally, avoidance forces are applied to the agent's acceleration based on the type of collision detected.

// When the left mouse button is pressed, the code adds the current mouse position as a new obstacle position to the vector.

// The code then proceeds to draw the agent, obstacles, and whiskers using various functions provided by the graphics library.

// Finally, the code checks if the grave(`~`) key is pressed to toggle the use of the graphical user interface. If the GUI is enabled, the rlImGuiBegin and rlImGuiEnd functions are called to render the GUI elements.

// The game loop continues until the window is closed, and at the end of the loop, the ImGui library is shut down, and the window is closed.
