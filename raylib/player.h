#pragma once
#include <raylib.h>
struct Player {
    Vector2 pos = { 0.0f, 0.0f };
    float base_speed = 300.0f;
    float run_speed = 500.f;
    short direction = 0;// типа как в юнити: -1 - влево, 1 - вправо
    short hp = 100;

    bool is_running = false;
    bool can_run = true;

    void Update();
    bool is_in_area(float &x);
    void Draw() const;


};
