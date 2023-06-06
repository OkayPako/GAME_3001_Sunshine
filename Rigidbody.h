#pragma once

struct Rigidbody
{
    Vector2 pos{};          // Position
    Vector2 vel{};          // Velocity
    Vector2 acc{};          // Acceleration

    Vector2 dir{};          // Direction
    float angularSpeed;     // Angular speed (in degrees per second)
};
