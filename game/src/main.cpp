#include "rlImGui.h"
#include "raylib.h"
#include <vector>
#include <cmath>

const int screenWidth = 1280;
const int screenHeight = 720;

class Sprite 
{
public:
    Texture2D texture;
    Vector2 position;
    int width;
    int height;

    void SetWidth(int w) 
    {
        width = w;
    }

    void SetHeight(int h) 
    {
        height = h;
    }

    void Draw() const 
    {
        DrawTexturePro(texture, { 0, 0, (float)texture.width, (float)texture.height }, { position.x, position.y, (float)width, (float)height }, { 0, 0 }, 0.0f, WHITE);
    }
};

class Rigidbody 
{
public:
    Vector2 position;
    Vector2 velocity;
};

class Agent 
{
public:
    Rigidbody rigidbody;
    Sprite sprite;
    float maxSpeed;
    float maxAcceleration;

    Vector2 Subtract(const Vector2& v1, const Vector2& v2) 
    {
        return { v1.x - v2.x, v1.y - v2.y };
    }

    float Length(const Vector2& vector) 
    {
        return std::sqrt(vector.x * vector.x + vector.y * vector.y);
    }

    Vector2 Normalize(const Vector2& vector) 
    {
        float length = Length(vector);
        if (length != 0) 
        {
            return { vector.x / length, vector.y / length };
        }
        else 
        {
            return { 0, 0 };
        }
    }

    Vector2 Scale(const Vector2& vector, float scalar) 
    {
        return { vector.x * scalar, vector.y * scalar };
    }

    void Update(float deltaTime) 
    {
        rigidbody.position.x += rigidbody.velocity.x * deltaTime;
        rigidbody.position.y += rigidbody.velocity.y * deltaTime;

        // Check screen bounds
        if (rigidbody.position.x < 0) 
        {
            rigidbody.position.x = 0;
            rigidbody.velocity.x = 0;
        }
        else if (rigidbody.position.x > screenWidth - sprite.width) 
        {
            rigidbody.position.x = screenWidth - sprite.width;
            rigidbody.velocity.x = 0;
        }

        if (rigidbody.position.y < 0) 
        {
            rigidbody.position.y = 0;
            rigidbody.velocity.y = 0;
        }
        else if (rigidbody.position.y > screenHeight - sprite.height) 
        {
            rigidbody.position.y = screenHeight - sprite.height;
            rigidbody.velocity.y = 0;
        }
    }

    Vector2 Seek(const Vector2& targetPosition) 
    {
        Vector2 desiredVelocity = Subtract(targetPosition, rigidbody.position);
        desiredVelocity = Normalize(desiredVelocity);
        desiredVelocity = Scale(desiredVelocity, maxSpeed);

        Vector2 steering = Subtract(desiredVelocity, rigidbody.velocity);
        steering.x = Clamp(steering.x, -maxAcceleration, maxAcceleration);
        steering.y = Clamp(steering.y, -maxAcceleration, maxAcceleration);

        return steering;
    }

    Vector2 Flee(const Vector2& targetPosition) 
    {
        Vector2 desiredVelocity = Subtract(rigidbody.position, targetPosition);
        desiredVelocity = Normalize(desiredVelocity);
        desiredVelocity = Scale(desiredVelocity, maxSpeed);

        Vector2 steering = Subtract(desiredVelocity, rigidbody.velocity);
        steering.x = Clamp(steering.x, -maxAcceleration, maxAcceleration);
        steering.y = Clamp(steering.y, -maxAcceleration, maxAcceleration);

        return steering;
    }

    float Clamp(float value, float min, float max) 
    {
        if (value < min) 
        {
            return min;
        }
        else if (value > max) 
        {
            return max;
        }
        else 
        {
            return value;
        }
    }
};

int main() 
{
    InitWindow(screenWidth, screenHeight, "Seek and Flee");

    std::vector<Agent> agents;

    // Create agent
    Agent agent;
    agent.rigidbody.position = { 400, 225 };
    agent.rigidbody.velocity = { 0, 0 };
    agent.sprite.texture = LoadTexture("../game/assets/textures/magikarp.png");
    agent.sprite.SetWidth(100);
    agent.sprite.SetHeight(100);
    agent.sprite.position = agent.rigidbody.position;
    agent.maxSpeed = 400.0f;
    agent.maxAcceleration = 800.0f;
    agents.push_back(agent);

    SetTargetFPS(60);

    while (!WindowShouldClose()) 
    {
        // Update
        float deltaTime = GetFrameTime();

        // Handle agent behavior
        for (Agent& agent : agents) 
        {
            Vector2 mousePosition = GetMousePosition();

            Vector2 agentAcceleration;
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) 
            {
                agentAcceleration = agent.Seek(mousePosition);
            }
            else 
            {
                agentAcceleration = agent.Flee(mousePosition);
            }

            agent.rigidbody.velocity.x += agentAcceleration.x * deltaTime;
            agent.rigidbody.velocity.y += agentAcceleration.y * deltaTime;

            // Clamp velocity to max speed
            float agentSpeed = agent.Length(agent.rigidbody.velocity);
            if (agentSpeed > agent.maxSpeed) 
            {
                agent.rigidbody.velocity = agent.Scale(agent.rigidbody.velocity, agent.maxSpeed / agentSpeed);
            }

            // Update agent
            agent.Update(deltaTime);
            agent.sprite.position = agent.rigidbody.position;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw agents
        for (const Agent& agent : agents) 
        {
            agent.sprite.Draw();
        }

        EndDrawing();
    }

    // Unload resources
    for (Agent& agent : agents) 
    {
        UnloadTexture(agent.sprite.texture);
    }

    CloseWindow();

    return 0;
}