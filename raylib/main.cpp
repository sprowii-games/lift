#include "raylib.h"
#include "player.h"
#include "camera.h"
#include "menu.h"

int main() {
    const int monitorWidth = GetMonitorWidth(GetCurrentMonitor());
    const int monitorHeight = GetMonitorHeight(GetCurrentMonitor());
    
    // Set the flag
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(monitorWidth, monitorHeight, "LIFT");
    SetTargetFPS(60);
    Texture2D door_opened = LoadTexture("textures/elevator_opened.png");
    Player player;
    Cam camera;
    Menu menu;
    //Font myfont = LoadFont("");
    camera.Init();
    menu.Init();
    bool islevelcompleted = false;
    const int width = GetScreenWidth();
    const int height = GetScreenHeight();
    Vector2 rectangle = { width / 2, height / 2 };
    Color rect = RED;
    short level = -4;
    bool is_puzzle_active = false;
    float camera_speed = 5.0f;
    player.pos = { (float)width / 100.0f * 16.6f,(float)height / 100.0f * 75 };
    while (!WindowShouldClose()) {
        // Проверяем состояние игры
        if (menu.state == MENU) {
            menu.Update();
            BeginDrawing();
            menu.Draw();
            EndDrawing();
        }
        else if (menu.state == PLAYING) {
            if (islevelcompleted) {
                level++;
                islevelcompleted = false;
            }
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
            DrawTexture(door_opened, (float)width / 100.0f * 5.0f,(float)height / 100.0f * 50.0f, WHITE);
            player.Draw();
            EndMode2D();
            int x = (width / 2) - (800 / 2);
            int y = (height / 2) - (600 / 2);
            DrawFPS(10, 10);
            if (is_puzzle_active) {
                DrawRectangle(x, y, 800, 600, BLACK);
            }
            
            DrawText("there are two chairs...", x + 3, y - 2, 33, RAYWHITE);
            EndDrawing();
        }
        else if (menu.state == EXIT) {
            break; // выходим из цикла
        }
    }
    UnloadTexture(door_opened);
    CloseWindow();
    return 0;
}