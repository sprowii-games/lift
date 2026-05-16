#pragma once
#include <raylib.h>

struct Player {
    Vector2 pos = { 0.0f, 0.0f };
    float base_speed = 300.0f;
    float run_speed = 500.0f;
    short direction = 0;   // -1 left, 1 right, 0 still
    short hp = 100;
    bool is_running = false;
    bool can_run = true;
    float animTimer = 0.0f;

    void Update();
    bool is_in_area(float x) const;
    void Draw() const;
};
