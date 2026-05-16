#include "player.h"
#include <cmath>

void Player::Update() {
    float dt = GetFrameTime();
    animTimer += dt;

    direction = 0;
    bool moving = false;

    if (IsKeyDown(KEY_D)) {
        direction = 1;
        moving = true;
    }
    if (IsKeyDown(KEY_A)) {
        direction = -1;
        moving = true;
    }

    is_running = false;
    if (IsKeyDown(KEY_LEFT_SHIFT) && moving && can_run) {
        is_running = true;
    }

    if (moving) {
        float speed = is_running ? run_speed : base_speed;
        pos.x += speed * direction * dt;
    }
}

bool Player::is_in_area(float x) const {
    return (x - 15.0f < pos.x && pos.x < x + 100.0f);
}

void Player::Draw() const {
    float bob = 0.0f;
    float breatheW = 1.0f * sinf(animTimer * 3.0f);

    if (direction != 0) {
        bob = 2.0f * sinf(animTimer * 10.0f);
    }

    // Shadow
    DrawEllipse(pos.x, pos.y + 36, 28, 10, Fade(BLACK, 0.3f));

    // Coat tail (sways with movement)
    float sway = sinf(animTimer * 10.0f + PI / 2.0f) * 4.0f;
    Vector2 t1 = Vector2{ pos.x - 6.0f + sway * 0.5f, pos.y + 18.0f + bob };
    Vector2 t2 = Vector2{ pos.x + 6.0f + sway * 0.5f, pos.y + 18.0f + bob };
    Vector2 t3 = Vector2{ pos.x + sway, pos.y + 32.0f + bob };
    DrawTriangle(t1, t2, t3, Color{ 230, 230, 245 });

    // Legs
    float legSwingL = 0.0f;
    float legSwingR = 0.0f;
    if (direction != 0) {
        legSwingL = sinf(animTimer * 10.0f) * 20.0f;
        legSwingR = sinf(animTimer * 10.0f + PI) * 20.0f;
    }

    Rectangle legL = Rectangle{ pos.x - 9.0f, pos.y + 18.0f + bob, 8.0f, 22.0f };
    Rectangle legR = Rectangle{ pos.x + 1.0f, pos.y + 18.0f + bob, 8.0f, 22.0f };
    DrawRectanglePro(legL, Vector2{ 4.0f, 0.0f }, legSwingL, Color{ 40, 40, 60 });
    DrawRectanglePro(legR, Vector2{ 4.0f, 0.0f }, legSwingR, Color{ 40, 40, 60 });

    // Body / lab coat
    Rectangle body = Rectangle{ pos.x - 13.0f - breatheW * 0.5f, pos.y - 18.0f + bob, 26.0f + breatheW, 36.0f };
    DrawRectangleRec(body, Color{ 230, 230, 245 });

    // Head
    Vector2 head = Vector2{ pos.x, pos.y - 30.0f + bob };
    DrawCircleV(head, 14.0f, Color{ 220, 180, 160 });

    // Hair
    DrawCircleV(Vector2{ head.x - 6.0f, head.y - 10.0f }, 4.0f, Color{ 60, 50, 50 });
    DrawCircleV(Vector2{ head.x + 6.0f, head.y - 10.0f }, 4.0f, Color{ 60, 50, 50 });
    DrawCircleV(Vector2{ head.x, head.y - 12.0f }, 5.0f, Color{ 60, 50, 50 });

    // Arms (swing opposite to legs)
    float armSwingL = 0.0f;
    float armSwingR = 0.0f;
    if (direction != 0) {
        armSwingL = sinf(animTimer * 10.0f + PI) * 25.0f;
        armSwingR = sinf(animTimer * 10.0f) * 25.0f;
    }

    Rectangle armL = Rectangle{ pos.x - 11.0f, pos.y - 12.0f + bob, 6.0f, 20.0f };
    Rectangle armR = Rectangle{ pos.x + 5.0f, pos.y - 12.0f + bob, 6.0f, 20.0f };
    DrawRectanglePro(armL, Vector2{ 3.0f, 0.0f }, armSwingL, Color{ 200, 200, 210 });
    DrawRectanglePro(armR, Vector2{ 3.0f, 0.0f }, armSwingR, Color{ 200, 200, 210 });

    // Eyes (always open, always black)
    float eyeShift = (float)direction * 3.0f;
    DrawEllipse(head.x - 5.0f + eyeShift, head.y - 2.0f, 3.0f, 4.0f, BLACK);
    DrawEllipse(head.x + 5.0f + eyeShift, head.y - 2.0f, 3.0f, 4.0f, BLACK);
}
