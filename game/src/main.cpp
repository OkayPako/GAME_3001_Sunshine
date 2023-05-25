#include "rlImGui.h"
#include "raylib.h"
#include "Math.h"
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

    Vector2 FleeFromObjects(const Vector2& targetPosition, const Vector2& mousePosition, const std::vector<Vector2>& objectPositions)
    {
        // Calculate desired velocity to flee from target position
        Vector2 desiredVelocity = Subtract(rigidbody.position, targetPosition);
        desiredVelocity = Normalize(desiredVelocity);
        desiredVelocity = Scale(desiredVelocity, maxSpeed);

        // Calculate steering force to flee from objects and mouse position
        Vector2 objectSteering = { 0, 0 };
        for (const Vector2& objectPosition : objectPositions)
        {
            Vector2 objectVelocity = Subtract(objectPosition, rigidbody.position); // Calculate velocity away from the object
            float objectDistance = Length(objectVelocity);
            if (objectDistance < 200.0f) // Flee from objects within a certain distance
            {
                objectVelocity = Normalize(objectVelocity);
                objectVelocity = Scale(objectVelocity, maxSpeed);
                objectVelocity = Scale(objectVelocity, 1.0f - (objectDistance / 200.0f));
                objectSteering = Add(objectSteering, objectVelocity); // Add object steering to the total steering
            }
        }

        Vector2 mouseSteering = Subtract(rigidbody.position, mousePosition);
        mouseSteering = Normalize(mouseSteering);
        mouseSteering = Scale(mouseSteering, maxSpeed);

        // Combine desired velocity, object steering, and mouse steering
        Vector2 steering = Subtract(desiredVelocity, rigidbody.velocity);
        steering.x = Clamp(steering.x, -maxAcceleration, maxAcceleration);
        steering.y = Clamp(steering.y, -maxAcceleration, maxAcceleration);

        // Add object steering and mouse steering to the total steering
        steering = Add(steering, objectSteering);
        steering = Add(steering, mouseSteering);

        return steering;
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
    agent.sprite.SetWidth(50);
    agent.sprite.SetHeight(50);
    agent.sprite.position = agent.rigidbody.position;
    agent.maxSpeed = 400.0f;
    agent.maxAcceleration = 800.0f;
    agents.push_back(agent);

    // Create objects to flee from
    std::vector<Vector2> objectsToFlee;
    objectsToFlee.push_back({ 200, 200 });
    objectsToFlee.push_back({ 600, 400 });
    objectsToFlee.push_back({ 800, 100 });

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Update
        float deltaTime = GetFrameTime();

        // Handle agent behavior
        for (Agent& agent : agents)
        {
            Vector2 mousePosition = GetMousePosition();
            std::vector<Vector2> objectPositions;

            // Add object positions to the vector
            for (const Vector2& objectPosition : objectsToFlee)
            {
                objectPositions.push_back(objectPosition);
            }

            Vector2 agentAcceleration = { 0, 0 };

            // Check if left mouse button is pressed
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                // Seek towards the mouse position
                agentAcceleration = agent.Seek(mousePosition);
            }
            else
            {
                // Flee from the cursor position and objects
                agentAcceleration = agent.FleeFromObjects(mousePosition, mousePosition, objectPositions);
            }

            agent.rigidbody.velocity.x += agentAcceleration.x * deltaTime;
            agent.rigidbody.velocity.y += agentAcceleration.y * deltaTime;

            // Clamp velocity to max speed
            float agentSpeed = Length(agent.rigidbody.velocity);
            if (agentSpeed > agent.maxSpeed)
            {
                agent.rigidbody.velocity = Scale(agent.rigidbody.velocity, agent.maxSpeed / agentSpeed);
            }

            // Update agent
            agent.Update(deltaTime);
            agent.sprite.position = agent.rigidbody.position;
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        // Draw agents
        for (const Agent& agent : agents)
        {
            agent.sprite.Draw();
        }

        // Draw objects to flee from
        for (const Vector2& objectPosition : objectsToFlee)
        {
            DrawCircle(objectPosition.x, objectPosition.y, 10, BROWN);
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
