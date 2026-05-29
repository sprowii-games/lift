#include "raylib.h"
#include "player.h"
#include "camera.h"
#include "menu.h"
#include "assets.h"
#include "level1.h"
#include "level4.h"
#include "level6.h"
#include "level8.h"

enum TransitState { TRANSIT_NONE, TRANSIT_FADE_OUT, TRANSIT_SHAKE, TRANSIT_FADE_IN };

static void ClampPlayerToLevelBounds(Player& player, int screenW) {
    const float leftWall = 45.0f;
    const float rightWall = screenW - 45.0f;
    if (player.pos.x < leftWall) player.pos.x = leftWall;
    if (player.pos.x > rightWall) player.pos.x = rightWall;
}

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
    SetExitKey(0);               // отключаем авто-закрытие по ESC — обрабатываем вручную
    SetTargetFPS(60);

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

    Level1 level1;
    Level4 level4;
    Level6 level6;
    Level8 level8;


    const int levelCount = 4;
    int currentLevel = 0;
    bool gameCompleted = false;
    bool gameBadEnd = false;

    TransitState transitState = TRANSIT_NONE;
    float transitTimer = 0.0f;
    float transitFadeAlpha = 0.0f;
    bool elevatorSoundPlayed = false;

    int currentMusicTrack = -1;

    level1.Init(width, height);
    level4.Init(width, height);
    level6.Init(width, height);
    level8.Init(width, height);
    player.pos = { width * 0.166f, height * 0.825f };

    while (!WindowShouldClose()) {
        int desiredTrack = -1;
        if (menu.state == MENU || menu.state == CREDITS) desiredTrack = 0;
        else if (gameCompleted) desiredTrack = -1;
        else if (transitState == TRANSIT_NONE) {
            if (currentLevel == 0) desiredTrack = 1;
            else if (currentLevel == 1) desiredTrack = 2;
            else if (currentLevel == 2) desiredTrack = 3;
            else if (currentLevel == 3) desiredTrack = 4;
        }

        if (desiredTrack != currentMusicTrack) {
            switch (currentMusicTrack) {
                case 0: StopMusicStream(assets.music_menu); break;
                case 1: StopMusicStream(assets.music_beginning); break;
                case 2: StopMusicStream(assets.music_first_two); break;
                case 3: StopMusicStream(assets.music_second_two); break;
                case 4: StopMusicStream(assets.music_end); break;
            }
            currentMusicTrack = desiredTrack;
            switch (currentMusicTrack) {
                case 0: PlayMusicStream(assets.music_menu); break;
                case 1: PlayMusicStream(assets.music_beginning); break;
                case 2: PlayMusicStream(assets.music_first_two); break;
                case 3: PlayMusicStream(assets.music_second_two); break;
                case 4: PlayMusicStream(assets.music_end); break;
            }
        }

        switch (currentMusicTrack) {
            case 0: UpdateMusicStream(assets.music_menu); break;
            case 1: UpdateMusicStream(assets.music_beginning); break;
            case 2: UpdateMusicStream(assets.music_first_two); break;
            case 3: UpdateMusicStream(assets.music_second_two); break;
            case 4: UpdateMusicStream(assets.music_end); break;
        }

        if (menu.state == MENU) {
            menu.Update(assets.font);
            if (IsKeyPressed(KEY_ESCAPE)) menu.state = EXIT;

            BeginDrawing();
            menu.Draw(assets.font);
            EndDrawing();
        }
        else if (menu.state == PLAYING) {
            if (!gameCompleted) {
                if (transitState == TRANSIT_NONE) {
                    if (IsKeyPressed(KEY_R)) {
                        switch (currentLevel) {
                            case 0: level1.Init(width, height); break;
                            case 1: level4.Init(width, height); break;
                            case 2: level6.Init(width, height); break;
                            case 3: level8.Init(width, height); break;
                        }
                        player.pos = { width * 0.166f, height * 0.82f };
                        player.can_move = true;
                    }

                    player.Update();
                    ClampPlayerToLevelBounds(player, width);
                    camera.Update(player, camera_speed);

                    bool levelCompleted = false;
                    bool levelBadEnd = false;
                    switch (currentLevel) {
                        case 0: level1.Update(player, camera); levelCompleted = level1.completed; levelBadEnd = level1.badEnding; break;
                        case 1: level4.Update(player); levelCompleted = level4.completed; levelBadEnd = level4.badEnding; break;
                        case 2: level6.Update(player); levelCompleted = level6.completed; levelBadEnd = level6.badEnding; break;
                        case 3: level8.Update(player); levelCompleted = level8.completed; levelBadEnd = level8.badEnding; break;
                    }

                    if (levelCompleted) {
                        if (levelBadEnd) {
                            gameBadEnd = true;
                            gameCompleted = true;
                        } else {
                            transitState = TRANSIT_FADE_OUT;
                            transitTimer = 0.0f;
                            transitFadeAlpha = 0.0f;
                            elevatorSoundPlayed = false;
                        }
                    }
                } else {
                    float dt = GetFrameTime();
                    transitTimer += dt;

                    if (transitState == TRANSIT_FADE_OUT) {
                        transitFadeAlpha += dt * 1.0f;
                        if (transitFadeAlpha >= 1.0f) {
                            transitFadeAlpha = 1.0f;
                            transitState = TRANSIT_SHAKE;
                            transitTimer = 0.0f;
                        }
                    }
                    else if (transitState == TRANSIT_SHAKE) {
                        if (!elevatorSoundPlayed) {
                            PlaySound(assets.sfx_elevator_up);
                            elevatorSoundPlayed = true;
                            camera.AddShake(2.0f);
                            camera.Update(player, camera_speed);
                            currentLevel++;
                            if (currentLevel >= levelCount) {
                                gameCompleted = true;
                                transitState = TRANSIT_NONE;
                            } else {
                                switch (currentLevel) {
                                    case 1: level4.Init(width, height); break;
                                    case 2: level6.Init(width, height); break;
                                    case 3: level8.Init(width, height); break;
                                }
                                player.pos = { width * 0.166f, height * 0.82f };
                                ClampPlayerToLevelBounds(player, width);
                            }
                        }

                        if (!gameCompleted) {
                            // тряска всё время пока звук играет
                            camera.AddShake(0.5f);
                            camera.Update(player, camera_speed);

                            // переход когда звук кончится (минимум 0.3с на всякий случай)
                            if (transitTimer > 0.3f && !IsSoundPlaying(assets.sfx_elevator_up)) {
                                transitState = TRANSIT_FADE_IN;
                                transitTimer = 0.0f;
                            }
                        }
                    }
                    else if (transitState == TRANSIT_FADE_IN) {
                        transitFadeAlpha -= dt * 1.0f;
                        if (transitFadeAlpha <= 0.0f) {
                            transitFadeAlpha = 0.0f;
                            transitState = TRANSIT_NONE;
                        }
                    }
                }

                BeginDrawing();
                ClearBackground(Color{8, 8, 12, 255});

                BeginMode2D(camera.cam);
                switch (currentLevel) {
                    case 0: level1.DrawWorld(player, assets); break;
                    case 1: level4.Draw(player, assets); break;
                    case 2: level6.Draw(player, assets); break;
                    case 3: level8.Draw(player, assets); break;
                }
                EndMode2D();

                // Виньетка: включается после 1-го этажа, выключается на крыше
                if (currentLevel >= 1 && currentLevel <= 2) {
                    bool flip = (player.lastDirection == -1);   // lastDirection — стабильнее, не сбрасывается в 0
                    Texture2D v = assets.tex_vignette;
                    if (v.id != 0) {
                        Rectangle src = { 0.0f, 0.0f, flip ? -(float)v.width : (float)v.width, (float)v.height };
                        Rectangle dst = { 0.0f, 0.0f, (float)width, (float)height };
                        DrawTexturePro(v, src, dst, {0.0f, 0.0f}, 0.0f, WHITE);
                    }
                }

                if (currentLevel == 0) {
                    level1.DrawTerminalUI(player, assets);
                }

                if (currentLevel == 1) {
                    level4.DrawUI(assets);
                } else if (currentLevel == 2) {
                    level6.DrawUI(assets);
                } else if (currentLevel == 3) {
                    level8.DrawUI(assets);
                }

                if (transitState != TRANSIT_NONE) {
                    DrawRectangle(0, 0, width, height, Fade(BLACK, transitFadeAlpha));
                }

                DrawFPS(10, 10);
                RuText(assets.font, TextFormat("ЭТАЖ %d / %d", currentLevel + 1, levelCount), 10, 40, 20, DARKGRAY);
                RuText(assets.font, u8"A/D — ходить | SHIFT — бег | E — действие | R — рестарт", 10, height - 30, 16, DARKGRAY);
                EndDrawing();
            }
            else {
                if (currentMusicTrack != -1) {
                    switch (currentMusicTrack) {
                        case 0: StopMusicStream(assets.music_menu); break;
                        case 1: StopMusicStream(assets.music_beginning); break;
                        case 2: StopMusicStream(assets.music_first_two); break;
                        case 3: StopMusicStream(assets.music_second_two); break;
                        case 4: StopMusicStream(assets.music_end); break;
                    }
                    currentMusicTrack = -1;
                }

                BeginDrawing();
                ClearBackground(BLACK);

                if (gameBadEnd) {
                    RuText(assets.font, "ТЫ УЛЕТЕЛ.", width / 2 - 120, height / 2 - 60, 40, GOLD);
                    RuText(assets.font, "Но был ли там вообще вертолёт?", width / 2 - 240, height / 2, 26, RAYWHITE);
                    RuText(assets.font, "Ветер, шум лопастей, пустота под ногами —", width / 2 - 290, height / 2 + 40, 22, GRAY);
                    RuText(assets.font, "всё казалось настоящим. Кроме земли.", width / 2 - 240, height / 2 + 72, 22, GRAY);
                }
                else {
                    RuText(assets.font, "ТЫ ВЫШЕЛ.", width / 2 - 150, height / 2 - 40, 40, GOLD);
                    RuText(assets.font, "Не герой. Не учёный. Просто человек.", width / 2 - 250, height / 2 + 20, 22, RAYWHITE);
                    RuText(assets.font, "И этого достаточно.", width / 2 - 120, height / 2 + 55, 22, GRAY);
                }
                RuText(assets.font, "ESC — выйти.", width / 2 - 60, height / 2 + 130, 18, DARKGRAY);

                if (IsKeyPressed(KEY_ESCAPE)) break;

                EndDrawing();
            }
        }
        else if (menu.state == CREDITS) {
            if (IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                menu.state = MENU;
            }

            BeginDrawing();
            ClearBackground(Color{8, 8, 14, 255});

            int sw = GetScreenWidth();
            int sh = GetScreenHeight();

            int tw;
            tw = RuMeasure(assets.font, u8"КРЕДИТЫ", 48);
            RuText(assets.font, u8"КРЕДИТЫ", (sw - tw) / 2, 150, 48, RAYWHITE);

            tw = RuMeasure(assets.font, u8"Музыка", 32);
            RuText(assets.font, u8"Музыка", (sw - tw) / 2, 280, 32, GOLD);
            tw = RuMeasure(assets.font, "chajamakesmusic", 28);
            RuText(assets.font, "chajamakesmusic", (sw - tw) / 2, 340, 28, RAYWHITE);
            tw = RuMeasure(assets.font, "crow shade", 28);
            RuText(assets.font, "crow shade", (sw - tw) / 2, 390, 28, RAYWHITE);

            tw = RuMeasure(assets.font, u8"ESC / клик — назад", 20);
            RuText(assets.font, u8"ESC / клик — назад", (sw - tw) / 2, sh - 80, 20, Color{100, 100, 110, 255});

            EndDrawing();
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
