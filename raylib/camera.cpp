#include "camera.h"
#include <cmath>

void Cam::Init() {
    cam = {};
    cam.zoom = defaultZoom;
    cam.rotation = 0.0f;
    speed = 5.0f;
    last_direction = 1;
    shake = 0.0f;
    float margin = (float)GetScreenWidth() / 100.0f * 83.33f;
    cam.offset = { (float)GetScreenWidth() - margin, (float)GetScreenHeight() * 0.83f };
    cam.target = { 0.0f, 0.0f };
}

void Cam::SnapTo(Vector2 pos) {
    cam.target = pos;
}

void Cam::AddShake(float amount) {
    shake = amount;
}

void Cam::Update(Player& pl, float cam_sp) {
    float dt = GetFrameTime();

    float runZoom = defaultZoom * 0.85f;   // при беге чуть отдаляем
    if (pl.is_running) {
        cam.zoom += (runZoom - cam.zoom) * cam_sp * dt;
    } else {
        cam.zoom += (defaultZoom - cam.zoom) * cam_sp * dt;
    }

    cam.target.x += (pl.pos.x - cam.target.x) * cam_sp * dt;
    cam.target.y += (pl.pos.y - cam.target.y) * cam_sp * dt * 0.5f;

    if (pl.direction != 0) {
        last_direction = pl.direction;
    }

    float margin = (float)GetScreenWidth() / 100.0f * 83.33f;
    float target_offset = (last_direction == 1) ? ((float)GetScreenWidth() - margin) : margin;
    cam.offset.x += (target_offset - cam.offset.x) * cam_sp * dt;

    if (shake > 0.0f) {
        float randomX = (float)GetRandomValue(-(int)(shake * 10.0f), (int)(shake * 10.0f)) * 0.1f;
        float randomY = (float)GetRandomValue(-(int)(shake * 10.0f), (int)(shake * 10.0f)) * 0.1f;
        cam.offset.x += randomX;
        cam.offset.y += randomY;
        shake -= dt * 2.0f;
        if (shake < 0.0f) shake = 0.0f;
    }
}
