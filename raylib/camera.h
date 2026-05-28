#pragma once
#include <raylib.h>
#include "player.h"

struct Cam {
    Camera2D cam;
    float speed;
    short last_direction = 1;
    float shake = 0.0f;
    float defaultZoom = 1.5f;   // зум камеры (>1 = ближе/меньше обзор, <1 = дальше/больше обзор)

    void Init();
    void SnapTo(Vector2 pos);
    void AddShake(float amount);
    void Update(Player& pl, float cam_sp);
};
