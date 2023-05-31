// This is all in the while loop

//Vector2 targetPosition;
//Vector2 position { 100,100 };
//Vector2 velocity = { 10.0, 0.0 };
//Vector2 acceleration = { 10.0f, 0.0f };

// 1. Do displacement first
// 2. Update the position of the target
// 3. Update the velocity accordingly

// Vector2 displacement = velocity * deltaTime;

// acceleration = (position2 - position1) / deltaTime; // This will make the bottom line make sense.
// position = position + velocity * deltaTime (we have displacement to represent this) + 0.5f * a * deltaTime * deltaTime; // updating position 
// position = position + displacement + 0.5f * acceleration * deltaTime * deltaTime;

// v = v + a * deltaTime; // updating velocity
// velocity = velocity + acceleration * deltaTime; // This is position2 = position1 + acceleration * deltaTime;

// DrawCricleV(position, 50, BLACK);
// DrawLineV(position, position + velocity, RED);
// DrawLineV(position, position + acceleration, GREEN);

// Raphael Flores 101139370

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

    // Constructor
    Agent(const Vector2& position, const Texture2D& texture, int width, int height, float speed, float acceleration)
        : maxSpeed(speed)
        , maxAcceleration(acceleration)
    {
        rigidbody.position = position;
        rigidbody.velocity = { 0, 0 };
        sprite.texture = texture;
        sprite.width = width;
        sprite.height = height;
        sprite.position = position;
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

    Vector2 Seek(const Vector2& targetPosition) // Calculates the steering force for an agent to move towards a target position.
    {
        // The desired velocity is calculated by subtracting the agent's current position (rigidbody.position) from the targetPosition and then normalizing the resulting vector. 
        // Normalizing a vector means scaling it to have a magnitude of 1 while preserving its direction.
        // Multiplying the normalized vector by maxSpeed determines the magnitude of the desired velocity.
        Vector2 desiredVelocity = Normalize(targetPosition - rigidbody.position) * maxSpeed; 

        // The steering force is then clamped or limited between -maxAcceleration and maxAcceleration using the Clamp function. 
        // This ensures that the agent's acceleration does not exceed the maximum allowed acceleration.
        Vector2 steering = Clamp(desiredVelocity - rigidbody.velocity, -maxAcceleration, maxAcceleration);

        return steering;
    }

    Vector2 Flee(const Vector2& targetPosition) // Calculates the steering force for an agent to move away from a target position and the objects.
    {
        // The desired velocity is calculated by subtracting the targetPosition from the agent's current position (rigidbody.position). 
        // This creates a vector pointing away from the target.
        Vector2 desiredVelocity = Normalize(rigidbody.position - targetPosition) * maxSpeed;

        Vector2 steering = Clamp(desiredVelocity - rigidbody.velocity, -maxAcceleration, maxAcceleration);

        return steering;
    }

    Vector2 Flee(const Vector2& agentPosition, const Vector2& targetPosition)
    {
        Vector2 desiredVelocity = Normalize(agentPosition - targetPosition) * maxSpeed;

        Vector2 steering = Clamp(desiredVelocity - rigidbody.velocity, -maxAcceleration, maxAcceleration);

        return steering;
    }
};

int main()
{
    InitWindow(screenWidth, screenHeight, "Raphael Flores 101139370 - Seek and Flee");

    // Background
    Texture2D sea = LoadTexture("../game/assets/textures/sea.png");
    sea.width = 1280;
    sea.height = 720;

    std::vector<Agent*> agents;
    Agent* agent;

    // Create agent using the constructor
    agent = (new Agent ({ 400, 225 }, LoadTexture("../game/assets/textures/magikarp.png"), 50, 50, 200.0f, 400.0f));
    agents.push_back(agent);

    agent = (new Agent ({ 600, 275 }, LoadTexture("../game/assets/textures/magikarp.png"), 50, 50, 400.0f, 400.0f));
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
        for (Agent* agent : agents)
        {
            Vector2 mousePosition = GetMousePosition();
            Vector2 agentAcceleration = { 0, 0 };

            // Add object positions to the vector
            std::vector<Vector2> objectPositions;
            for (const Vector2& objectPosition : objectsToFlee)
            {
                objectPositions.push_back(objectPosition);
            }

            // Check if left mouse button is pressed
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                // Seek towards the mouse position
                agentAcceleration = agent->Seek(mousePosition);
            }
            else
            {
                // Flee from the cursor position
                Vector2 fleeFromAgent = agent->Flee(agent->rigidbody.position, agent->rigidbody.position);
                Vector2 fleeFromMouse = agent->Flee(agent->rigidbody.position, mousePosition);

                // Flee from the object positions
                Vector2 fleeFromObjects = { 0, 0 };
                for (const Vector2& objectPosition : objectPositions)
                {
                    fleeFromObjects = fleeFromObjects + agent->Flee(agent->rigidbody.position, objectPosition);
                }

                agentAcceleration = fleeFromAgent + fleeFromMouse + fleeFromObjects;
            }
            
            agent->rigidbody.velocity.x += agentAcceleration.x * deltaTime;
            agent->rigidbody.velocity.y += agentAcceleration.y * deltaTime;

            // Clamp velocity to max speed
            float agentSpeed = Length(agent->rigidbody.velocity); // This function calculates the magnitude or length of a vector.
            if (agentSpeed > agent->maxSpeed)
            {
                // This function scales (or multiplies) a vector by a scalar value. 
                // Scale(agent.rigidbody.velocity, agent.maxSpeed / agentSpeed) scales the agent.rigidbody.velocity vector by a factor of agent.maxSpeed / agentSpeed. 
                // This operation adjusts the magnitude of the velocity vector to ensure it does not exceed the maximum speed (agent.maxSpeed).
                // This is us updating the new speed after the seek and flee functions are used.
                agent->rigidbody.velocity = Scale(agent->rigidbody.velocity, agent->maxSpeed / agentSpeed);
            }

            // Update agent
            agent->Update(deltaTime);
            agent->sprite.position = agent->rigidbody.position;
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        // Draw Background
        DrawTexture(sea, 0, 0, RAYWHITE);
        
        // Draw agents
        for (const Agent* agent : agents)
        {
            agent->sprite.Draw();
        }

        // Draw objects to flee from
        for (const Vector2& objectPosition : objectsToFlee)
        {
            DrawCircle(objectPosition.x, objectPosition.y, 10, BEIGE);
        }

        EndDrawing();
    }

    // Unload resources
    for (Agent* agent : agents)
    {
        UnloadTexture(agent->sprite.texture);
    }

    CloseWindow();

    return 0;
}
