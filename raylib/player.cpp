#include "player.h"
#include "assets.h"
#include <cmath>

void Player::Update() {
    float dt = GetFrameTime();
    animTimer += dt;

    bool prevMoving = (direction != 0);
    bool prevRunning = is_running;

    direction = 0;
    bool moving = false;

    if (IsKeyDown(KEY_D) && can_move) {
        direction = 1;
        moving = true;
    }
    if (IsKeyDown(KEY_A) && can_move) {
        direction = -1;
        moving = true;
    }

    is_running = false;
    if (IsKeyDown(KEY_LEFT_SHIFT) && moving && can_move) {
        is_running = true;
    }

    if (moving) {
        float speed = is_running ? run_speed : base_speed;
        pos.x += speed * direction * dt;
        lastDirection = direction;
    }

    bool curMoving = (direction != 0);
    bool curRunning = is_running;

    if (prevMoving != curMoving || prevRunning != curRunning) {
        currentFrame = 0;
        frameTimer = 0.0f;
    }

    float frameDuration;
    int maxFrames;
    if (direction == 0) {
        frameDuration = idleFrameDuration;
        maxFrames = 2;
    } else if (is_running) {
        frameDuration = runFrameDuration;
        maxFrames = 4;
    } else {
        frameDuration = walkFrameDuration;
        maxFrames = 4;
    }

    frameTimer += dt;
    if (frameTimer >= frameDuration) {
        frameTimer = 0.0f;
        currentFrame = (currentFrame + 1) % maxFrames;
    }
}

bool Player::is_in_area(float x) const {
    return (x - 15.0f < pos.x && pos.x < x + 100.0f);
}

void Player::stop_brother() {
    direction = 0;
    can_move = false;
    is_running = false;
}

void Player::Draw() const {
    DrawEllipse(pos.x, pos.y + 36, 28, 10, Fade(BLACK, 0.3f));
}

void Player::DrawSprite(const Assets& assets) const {
    DrawEllipse(pos.x, pos.y + 36, 28, 10, Fade(BLACK, 0.3f));

    Texture2D tex = {};
    int maxFrames = 1;
    if (direction == 0) {
        tex = assets.player_stand[0];
        maxFrames = 2;
    } else if (is_running) {
        tex = assets.player_run[0];
        maxFrames = 4;
    } else {
        tex = assets.player_walk[0];
        maxFrames = 4;
    }

    if (tex.id == 0) {
        Color bodyColor = is_running ? Color{255, 100, 50, 255} : Color{100, 180, 255, 255};
        int w = 32, h = 48;
        DrawRectangle((int)pos.x - w/2, (int)pos.y - h, w, h, bodyColor);
        DrawCircle(pos.x, pos.y - h - 6, 10, Color{255, 220, 180, 255});
        float bob = (direction != 0) ? 3.0f * sinf(animTimer * 12.0f) : 0.0f;
        DrawRectangle((int)pos.x - 10, (int)pos.y + (int)bob, 8, 14, Color{40, 40, 60, 255});
        DrawRectangle((int)pos.x + 2,  (int)pos.y - (int)bob, 8, 14, Color{40, 40, 60, 255});
        return;
    }

    int frame = currentFrame;
    if (frame < 0) frame = 0;
    if (frame >= maxFrames) frame = maxFrames - 1;

    if (direction == 0) {
        tex = assets.player_stand[frame];
    } else if (is_running) {
        tex = assets.player_run[frame];
    } else {
        tex = assets.player_walk[frame];
    }

    short drawDir = (direction == 0) ? lastDirection : direction;
    float flipX = (drawDir == -1) ? -1.0f : 1.0f;

    float scaledW = tex.width * spriteScale;
    float scaledH = tex.height * spriteScale;

    Rectangle src = { 0, 0, flipX * (float)tex.width, (float)tex.height };
    Rectangle dst = { pos.x - scaledW / 2.0f, pos.y - scaledH, scaledW, scaledH };
    Vector2 origin = { 0, 0 };
    DrawTexturePro(tex, src, dst, origin, 0.0f, WHITE);
}
