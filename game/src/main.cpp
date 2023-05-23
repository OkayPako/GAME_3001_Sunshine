#include "rlImGui.h"
#include "Math.h"
#include <vector>
#include <raylib.h>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Rigidbody class
class Rigidbody 
{
public:
    Vector2 position;
    Vector2 velocity;
};

// Sprite class
class Sprite 
{
public:
    Texture2D texture;
    Vector2 position;
    Rectangle bounds;

    void Draw() 
    {
        DrawTextureEx(texture, position, 0.0f, 1.0f, WHITE);
    }
};

// Agent class
class Agent 
{
public:
    Rigidbody rigidbody;
    Sprite sprite;
    float maxSpeed;
    float maxAcceleration;
};

// Update function for Rigidbody
void updateRigidbody(Rigidbody& rigidbody, float deltaTime, const Rectangle& bounds) 
{
    // Calculate new position based on velocity and time
    rigidbody.position.x += rigidbody.velocity.x * deltaTime;
    rigidbody.position.y += rigidbody.velocity.y * deltaTime;

    // Check window bounds
    if (rigidbody.position.x < 0) 
    {
        rigidbody.position.x = 0;
    }
    else if (rigidbody.position.x > SCREEN_WIDTH - bounds.width) 
    {
        rigidbody.position.x = SCREEN_WIDTH - bounds.width;
    }

    if (rigidbody.position.y < 0) 
    {
        rigidbody.position.y = 0;
    }
    else if (rigidbody.position.y > SCREEN_HEIGHT - bounds.height)
    {
        rigidbody.position.y = SCREEN_HEIGHT - bounds.height;
    }
}

Vector2 Vector2Normalize(const Vector2& vector) 
{
    float length = sqrtf(vector.x * vector.x + vector.y * vector.y);
    if (length != 0) 
    {
        return { vector.x / length, vector.y / length };
    }
    return { 0, 0 };
}

// Seek function for agents
Vector2 seek(const Vector2& agentPosition, const Vector2& agentVelocity, const Vector2& targetPosition, float maxAcceleration) 
{
    Vector2 desiredVelocity = Vector2Normalize(targetPosition - agentPosition) * maxAcceleration;
    Vector2 steer = desiredVelocity - agentVelocity;
    return steer;
}

// Flee function for agents
Vector2 flee(const Vector2& agentPosition, const Vector2& agentVelocity, const Vector2& targetPosition, float maxAcceleration) 
{
    Vector2 desiredVelocity = Vector2Normalize(agentPosition - targetPosition) * maxAcceleration;
    Vector2 steer = desiredVelocity - agentVelocity;
    return steer;
}

int main() 
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Agents Simulation");

    // Create agents
    std::vector<Agent> agents;
    Agent agent1;
    agent1.rigidbody.position = { 100, 100 };
    agent1.sprite.texture = LoadTexture("../game/assets/textures/goldfish.png");
    agent1.sprite.bounds = { 100, 100 };
    agent1.maxSpeed = 100.0f;
    agent1.maxAcceleration = 200.0f;
    agents.push_back(agent1);

    while (!WindowShouldClose()) 
    {
        // Update
        float deltaTime = GetFrameTime();

        for (auto& agent : agents) 
        {
            // Update agent's position and velocity
            updateRigidbody(agent.rigidbody, deltaTime, agent.sprite.bounds);

            // Seek towards mouse position while holding the mouse button
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) 
            {
                Vector2 targetPosition = GetMousePosition();
                Vector2 acceleration = seek(agent.rigidbody.position, agent.rigidbody.velocity, targetPosition, agent.maxAcceleration);
                agent.rigidbody.velocity.x += acceleration.x * deltaTime;
                agent.rigidbody.velocity.y += acceleration.y * deltaTime;
            }

            // Flee from another object in the world (e.g., a point)
            Vector2 fleeTargetPosition = { 500, 300 };
            Vector2 fleeAcceleration = flee(agent.rigidbody.position, agent.rigidbody.velocity, fleeTargetPosition, agent.maxAcceleration);
            agent.rigidbody.velocity.x += fleeAcceleration.x * deltaTime;
            agent.rigidbody.velocity.y += fleeAcceleration.y * deltaTime;

            // Limit agent's velocity to its maximum speed if needed
            // (not shown in the code, but you can add this if desired)
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw agents
        for (const auto& agent : agents) 
        {
            DrawTextureV(agent.sprite.texture, agent.sprite.position, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}