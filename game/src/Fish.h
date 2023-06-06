#pragma once
#include "Math.h"
#include "raylib.h"
#include "Rigidbody.h"
#include "main.cpp"

class Fish
{
public:
    Rigidbody rigidbody;
    Texture2D texture;
    float width;
    float height;
    float maxSpeed;
    float maxAcceleration;

    Fish(const Vector2& position, const Texture2D& texture, float width, float height, float speed, float acceleration)
        : maxSpeed(speed)
        , maxAcceleration(acceleration)
    {
        this->texture = texture;
        this->width = width;
        this->height = height;
        rigidbody.pos = position;
        rigidbody.vel = { 0, 0 };
        rigidbody.dir = { 1, 0 }; // Initial direction
        rigidbody.angularSpeed = 0.0f; // Initial angular speed
    }


    void Update(float deltaTime)
    {
        // Calculate desired velocity based on direction and maximum speed
        Vector2 desiredVel = Scale(rigidbody.dir, maxSpeed);

        // Calculate steering force based on desired velocity and current velocity
        Vector2 steering = Subtract(desiredVel, rigidbody.vel);

        // Limit the steering force to the maximum swim acceleration
        steering = Clamp(steering, -maxAcceleration, maxAcceleration);

        // Apply the steering force to the fish's acceleration
        rigidbody.acc = steering;

        // Update velocity based on acceleration
        rigidbody.vel.x += rigidbody.acc.x * deltaTime;
        rigidbody.vel.y += rigidbody.acc.y * deltaTime;

        // Limit velocity to the maximum swim speed
        rigidbody.vel = Clamp(rigidbody.vel, -maxSpeed, maxSpeed);

        // Update position based on velocity
        rigidbody.pos.x += rigidbody.vel.x * deltaTime;
        rigidbody.pos.y += rigidbody.vel.y * deltaTime;

        // Update direction based on velocity
        if (rigidbody.vel.x != 0 || rigidbody.vel.y != 0)
        {
            rigidbody.dir = Normalize(rigidbody.vel);
        }

        // Update sprite rotation based on direction
        float rotation = atan2f(rigidbody.dir.y, rigidbody.dir.x) * RAD2DEG;
        rigidbody.angularSpeed = rotation;

        // Check screen bounds
        if (rigidbody.pos.x < 0)
        {
            rigidbody.pos.x = 0;
            rigidbody.vel.x = 0;
        }
        else if (rigidbody.pos.x > SCREEN_WIDTH - width)
        {
            rigidbody.pos.x = SCREEN_WIDTH - width;
            rigidbody.vel.x = 0;
        }

        if (rigidbody.pos.y < 0)
        {
            rigidbody.pos.y = 0;
            rigidbody.vel.y = 0;
        }
        else if (rigidbody.pos.y > SCREEN_HEIGHT - height)
        {
            rigidbody.pos.y = SCREEN_HEIGHT - height;
            rigidbody.vel.y = 0;
        }
    }

    void Draw() const
    {
        Rectangle sourceRect = { 0, 0, (float)texture.width, (float)texture.height };
        Rectangle destRect = { rigidbody.pos.x, rigidbody.pos.y, width, height };
        Vector2 origin = { width / 2.0f, height / 2.0f };

        DrawTexturePro(texture, sourceRect, destRect, origin, rigidbody.angularSpeed, WHITE);
    }
};
