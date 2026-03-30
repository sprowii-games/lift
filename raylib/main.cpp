#include "raylib.h"
#include "player.h"
#include "camera.h"
#include "menu.h"

int main() {
    InitWindow(1920, 600, "LIFT");
    SetTargetFPS(60);
    ToggleFullscreen();
    Player player;
    Cam camera;
    Menu menu;

    camera.Init();
    menu.Init();

    Vector2 rectangle = { GetScreenWidth() / 2, GetScreenHeight() / 2 };
    Color rect = RED;
    bool is_puzzle_active = false;
    float camera_speed = 5.0f;
    player.pos = { (float)GetScreenWidth() / 6.0f,(float)GetScreenHeight() / 1.2f };
    while (!WindowShouldClose()) {
        // Проверяем состояние игры
        if (menu.state == MENU) {
            menu.Update();
            BeginDrawing();
            menu.Draw();
            EndDrawing();
        }
        else if (menu.state == PLAYING) {
            player.Update();

            if (player.is_in_area(rectangle.x)) {
                is_puzzle_active = true;
            }
            else {
                is_puzzle_active = false;
            }

            camera.Update(player, camera_speed);

            BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode2D(camera.cam);
            DrawRectangle(rectangle.x, rectangle.y, 50, 50, rect);
            if (is_puzzle_active) {
                DrawRectangle(0, 0, 800, 600, BLACK);
            }
            player.Draw();
            EndMode2D();

            DrawFPS(10, 10);
            EndDrawing();
        }
        else if (menu.state == EXIT) {
            break; // выходим из цикла
        }
    }

    CloseWindow();
    return 0;
}