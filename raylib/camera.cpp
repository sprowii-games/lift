#include "camera.h"

void Cam::Update(Player& pl, float cam_sp) {
    float dt = GetFrameTime();

    if (pl.is_running) {
        cam.zoom += (0.9f - cam.zoom) * cam_sp * dt;
    }
    else {
        cam.zoom += (1.0f - cam.zoom) * cam_sp * dt;
    }

    // Camera target smoothly follows player
    cam.target.x += (pl.pos.x - cam.target.x) * cam_sp * dt;

    // Update last direction if player is moving
    if (pl.direction != 0) {
        last_direction = pl.direction;
    }

    // Calculate target offset based on last direction
    float target_offset;
    if (last_direction == 1) {
        target_offset = (float)GetScreenWidth() / 100.0f * 16.66f;
    }
    else {
        target_offset = (float)GetScreenWidth() / 100.0f * 83.33f;
    }

    // Smoothly interpolate offset to target
    cam.offset.x += (target_offset - cam.offset.x) * cam_sp * dt;
}

void Cam::Init() {
    cam = { 0 };
    cam.zoom = 1.0f;
    speed = 5.0f;
    last_direction = 1;
    cam.rotation = 0.0f;
    cam.offset = { 0.0f, 0.0f };
    cam.target = { 0.0f, 0.0f };
}