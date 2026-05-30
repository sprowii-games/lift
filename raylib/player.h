#pragma once
#include <raylib.h>

struct Assets;

struct Player {
    Vector2 pos = { 0.0f, 0.0f };
    float base_speed = 300.0f;
    float run_speed = 500.0f;
    short direction = 0;   // -1 left, 1 right, 0 still
    short hp = 100;
    bool is_running = false;
    bool can_move = true;
    float animTimer = 0.0f;

    int currentFrame = 0;
    float frameTimer = 0.0f;
    float walkFrameDuration = 0.12f;
    float runFrameDuration = 0.07f;
    float idleFrameDuration = 0.5f;
    short lastDirection = 1;
    float spriteScale = 0.2f;    // масштаб спрайта (1.0 = оригинал, 1.5 = крупнее)
    float shadowOffsetY = 4.0f; // смещение тени ВНИЗ от pos.y (меньше = тень ближе к ногам, выше)
    float shadowRadiusX = 20.0f; // ширина тени (полуширина эллипса, меньше = уже)
    float shadowRadiusY = 8.0f;  // высота тени (полувысота эллипса)

    void Update();
    bool is_in_area(float x) const;
    void Draw() const;
    void DrawSprite(const Assets& assets) const;
    void stop_brother();
};
