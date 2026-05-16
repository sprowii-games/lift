#include "raylib.h"
#include "player.h"
#include "camera.h"
#include "menu.h"
#include "assets.h"
#include "level1.h"
#include "level2.h"
#include "level4.h"
#include "level6.h"
#include "level8.h"

// ============================================================
//  LIFT — психологический хоррор в лифте
//
//  Чтобы добавить свой этаж:
//    1. Создай levelN.h по шаблону существующих
//    2. Добавь #include "levelN.h" сверху
//    3. Добавь LevelN levelN; ниже
//    4. Добавь кейсы в switch'и (Init, Update, Draw)
//    5. Поменяй levelCount
//
//  Чтобы добавить музыку/звуки:
//    1. Положи файлы в sounds/
//    2. Раскомментируй в assets.h то что нужно
//    3. В Update играй: PlaySound(assets.sfx_click)
//    4. Для музыки: UpdateMusicStream(assets.music_...) каждый кадр
// ============================================================

int main() {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(1920, 1080, "LIFT");
    SetTargetFPS(60);

    // Аудио — нужно для звуков и музыки (раскомментируй в assets.h)
    InitAudioDevice();

    const int width = GetScreenWidth();
    const int height = GetScreenHeight();

    Assets assets;
    LoadAssets(assets);

    Player player;
    Cam camera;
    Menu menu;

    camera.Init();
    menu.Init();

    float camera_speed = 5.0f;

    // --- Уровни ---
    Level1 level1;  // Этаж 1: А.У.Р.А. + Закон Ома
    Level2 level2;  // Этаж 2: Сортировка (Bubble/Merge/Quick)
    Level4 level4;  // Этаж 3: Квантовая лаборатория
    Level6 level6;  // Этаж 4: BFS/DFS в лабиринте
    Level8 level8;  // Этаж 5: Крыша — выбор

    const int levelCount = 5;
    int currentLevel = 0;
    bool gameCompleted = false;
    bool gameBadEnd = false;

    switch (currentLevel) {
        case 0: level1.Init(width, height); break;
        case 1: level2.Init(width, height); break;
        case 2: level4.Init(width, height); break;
        case 3: level6.Init(width, height); break;
        case 4: level8.Init(width, height); break;
    }

    player.pos = { width * 0.166f, height * 0.75f };

    while (!WindowShouldClose()) {
        if (menu.state == MENU) {
            menu.Update(assets.font);

            BeginDrawing();
            menu.Draw(assets.font);
            EndDrawing();
        }
        else if (menu.state == PLAYING) {
            if (!gameCompleted) {
                if (IsKeyPressed(KEY_R)) {
                    switch (currentLevel) {
                        case 0: level1.Init(width, height); break;
                        case 1: level2.Init(width, height); break;
                        case 2: level4.Init(width, height); break;
                        case 3: level6.Init(width, height); break;
                        case 4: level8.Init(width, height); break;
                    }
                    player.pos = { width * 0.166f, height * 0.75f };
                }

                player.Update();
                camera.Update(player, camera_speed);

                bool levelCompleted = false;
                bool levelBadEnd = false;
                switch (currentLevel) {
                    case 0: level1.Update(player); levelCompleted = level1.completed; levelBadEnd = level1.badEnding; break;
                    case 1: level2.Update(player); levelCompleted = level2.completed; levelBadEnd = level2.badEnding; break;
                    case 2: level4.Update(player); levelCompleted = level4.completed; levelBadEnd = level4.badEnding; break;
                    case 3: level6.Update(player); levelCompleted = level6.completed; levelBadEnd = level6.badEnding; break;
                    case 4: level8.Update(player); levelCompleted = level8.completed; levelBadEnd = level8.badEnding; break;
                }

                if (levelCompleted) {
                    camera.AddShake(0.5f);
                    if (levelBadEnd) {
                        gameBadEnd = true;
                        gameCompleted = true;
                    } else {
                        currentLevel++;
                        if (currentLevel >= levelCount) {
                            gameCompleted = true;
                        } else {
                            switch (currentLevel) {
                                case 0: level1.Init(width, height); break;
                                case 1: level2.Init(width, height); break;
                                case 2: level4.Init(width, height); break;
                                case 3: level6.Init(width, height); break;
                                case 4: level8.Init(width, height); break;
                            }
                            player.pos = { width * 0.166f, height * 0.75f };
                        }
                    }
                }

                BeginDrawing();
                ClearBackground(RAYWHITE);

                BeginMode2D(camera.cam);
                switch (currentLevel) {
                    case 0: level1.Draw(player, assets); break;
                    case 1: level2.Draw(player, assets); break;
                    case 2: level4.Draw(player, assets); break;
                    case 3: level6.Draw(player, assets); break;
                    case 4: level8.Draw(player, assets); break;
                }
                EndMode2D();

                DrawRectangle(12, 12, 180, 62, Color{ 0, 0, 0, 120 });
                DrawRectangleLines(12, 12, 180, 62, Color{ 230, 230, 230, 80 });
                DrawFPS(22, 18);
                RuText(assets.font, TextFormat("ЭТАЖ %d / %d", currentLevel + 1, levelCount), 22, 48, 20, RAYWHITE);

                const char* controls = u8"A/D — ходить | SHIFT — бег | E — действие | R — рестарт";
                int controlsW = RuMeasure(assets.font, controls, 16);
                int controlsX = (width - controlsW) / 2;
                int controlsY = height - 44;
                DrawRectangle(controlsX - 16, controlsY - 6, controlsW + 32, 30, Color{ 0, 0, 0, 120 });
                RuText(assets.font, controls, controlsX, controlsY, 16, RAYWHITE);
                EndDrawing();
            }
            else {
                BeginDrawing();
                ClearBackground(BLACK);

                if (gameBadEnd) {
                    const char* t1 = "ТЫ УЛЕТЕЛ...";
                    const char* t2 = "Но был ли вертолёт на самом деле?";
                    const char* t3 = "Ветер кажется реальным. Падение кажется реальным.";
                    const char* t4 = "Но не земля.";
                    RuText(assets.font, t1, (width - RuMeasure(assets.font, t1, 40)) / 2, height / 2 - 90, 40, GOLD);
                    RuText(assets.font, t2, (width - RuMeasure(assets.font, t2, 26)) / 2, height / 2 - 24, 26, RAYWHITE);
                    RuText(assets.font, t3, (width - RuMeasure(assets.font, t3, 22)) / 2, height / 2 + 18, 22, GRAY);
                    RuText(assets.font, t4, (width - RuMeasure(assets.font, t4, 22)) / 2, height / 2 + 52, 22, GRAY);
                }
                else {
                    const char* win1 = "ИГРА ПРОЙДЕНА";
                    const char* win2 = "Ты принял правду. И это нормально.";
                    RuText(assets.font, win1, (width - RuMeasure(assets.font, win1, 40)) / 2, height / 2 - 28, 40, GOLD);
                    RuText(assets.font, win2, (width - RuMeasure(assets.font, win2, 22)) / 2, height / 2 + 32, 22, RAYWHITE);
                }
                const char* esc = "Нажмите ESC для выхода.";
                RuText(assets.font, esc, (width - RuMeasure(assets.font, esc, 18)) / 2, height / 2 + 126, 18, DARKGRAY);
                EndDrawing();
            }
        }
        else if (menu.state == EXIT) {
            break;
        }
    }

    UnloadAssets(assets);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
