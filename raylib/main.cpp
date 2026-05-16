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

                DrawFPS(10, 10);
                RuText(assets.font, TextFormat("ЭТАЖ %d / %d", currentLevel + 1, levelCount), 10, 40, 20, DARKGRAY);
                RuText(assets.font, u8"A/D — ходить | SHIFT — бег | E — действие | R — рестарт", 10, height - 30, 16, DARKGRAY);
                EndDrawing();
            }
            else {
                BeginDrawing();
                ClearBackground(BLACK);

                if (gameBadEnd) {
                    RuText(assets.font, "ТЫ УЛЕТЕЛ...", width / 2 - 180, height / 2 - 60, 40, GOLD);
                    RuText(assets.font, "Но был ли вертолёт на самом деле?", width / 2 - 260, height / 2, 26, RAYWHITE);
                    RuText(assets.font, "Ветер кажется реальным. Падение кажется реальным.", width / 2 - 290, height / 2 + 40, 22, GRAY);
                    RuText(assets.font, "Но не земля.", width / 2 - 90, height / 2 + 72, 22, GRAY);
                }
                else {
                    RuText(assets.font, "ИГРА ПРОЙДЕНА", width / 2 - 220, height / 2, 40, GOLD);
                    RuText(assets.font, "Ты принял правду. И это нормально.", width / 2 - 220, height / 2 + 50, 22, RAYWHITE);
                }
                RuText(assets.font, "Нажмите ESC для выхода.", width / 2 - 120, height / 2 + 130, 18, DARKGRAY);
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
