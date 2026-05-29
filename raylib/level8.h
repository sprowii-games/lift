#pragma once
#include "level.h"
#include "../utils.h"
#include <cmath>

// ЭТАЖ 5 «Крыша» — финальный выбор, без головоломки
// Слева: рюкзак (правда, принять реальность) — хорошая концовка
// Справа: вертолёт (галлюцинация, падение) — плохая концовка
struct Level8 {
    bool completed = false;
    bool badEnding = false;

    Vector2 backpackPos;   // рюкзак — слева, хорошая концовка
    Vector2 heliPos;       // вертолёт — справа, плохая концовка
    float notePosX;
    bool noteNear = false;
    bool noteOpen = false;
    bool choiceMade = false;
    float fadeAlpha = 0.0f;
    int hoveredChoice = -1; // -1 = ничего, 0 = рюкзак, 1 = вертолёт

    void Init(int screenW, int screenH) {
        completed = false;
        badEnding = false;
        noteNear = false;
        noteOpen = false;
        choiceMade = false;
        fadeAlpha = 0.0f;
        hoveredChoice = -1;

        backpackPos = { screenW * 0.20f, screenH * 0.66f };
        heliPos    = { screenW * 0.66f, screenH * 0.66f };

        notePosX = screenW * 0.50f;
    }

    void Update(Player& player) {
        noteNear = player.is_in_area(notePosX);
        hoveredChoice = -1;

        if (noteOpen) {
            player.stop_brother();
            if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_BACKSPACE)) {
                noteOpen = false;
                player.can_move = true;
            }
            return;
        }

        if (!choiceMade && noteNear && IsKeyPressed(KEY_E)) {
            noteOpen = true;
            player.stop_brother();
            return;
        }

        if (!choiceMade) {
            // подошёл к рюкзаку — хорошая концовка
            if (player.is_in_area(backpackPos.x)) {
                hoveredChoice = 0;
                if (IsKeyPressed(KEY_E)) {
                    choiceMade = true;
                    completed = true;
                }
            }

            // подошёл к вертолёту — плохая концовка
            if (player.is_in_area(heliPos.x)) {
                hoveredChoice = 1;
                if (IsKeyPressed(KEY_E)) {
                    choiceMade = true;
                    badEnding = true;
                }
            }
        }

        // плохая концовка: белый fade (галлюцинация рушится)
        if (badEnding && fadeAlpha < 1.0f) {
            fadeAlpha += GetFrameTime() * 0.7f;
            if (fadeAlpha >= 1.0f) {
                fadeAlpha = 1.0f;
                completed = true;
            }
        }
    }

    void Draw(Player& player, Assets& assets) {
        int scrW = GetScreenWidth();
        int scrH = GetScreenHeight();

        // --- фон: тёмно-синее ночное небо ---
        ClearBackground({ 15, 20, 40, 255 });

        // --- звёзды: 60 мерцающих точек ---
        static float starSeeds[60][2];
        static bool starsInit = false;
        if (!starsInit) {
            for (int s = 0; s < 60; s++) {
                starSeeds[s][0] = (float)GetRandomValue(0, 2000);
                starSeeds[s][1] = (float)GetRandomValue(0, 2000);
            }
            starsInit = true;
        }
        for (int s = 0; s < 60; s++) {
            float sx = fmodf(starSeeds[s][0] + GetTime() * 4.0f, (float)scrW);
            float sy = fmodf(starSeeds[s][1], scrH * 0.65f);
            float brightness = 0.4f + sinf(GetTime() * 2.0f + s) * 0.3f;
            DrawCircle(sx, sy, 1.5f, Fade(RAYWHITE, brightness));
        }

        // --- луна с «укусом» и свечением ---
        DrawCircle(scrW * 0.78f, scrH * 0.13f, 50, Fade(LIGHTGRAY, 0.3f));
        DrawCircle(scrW * 0.78f, scrH * 0.13f, 45, Fade({ 15, 20, 40, 255 }, 0.85f));
        DrawGlow({ scrW * 0.78f, scrH * 0.13f }, 80.0f, { 220, 230, 255, 100 }, { 220, 230, 255, 0 }, 8);

        // --- дальний город за краем крыши ---
        float roofY = scrH * 0.77f;
        for (int i = 0; i < 12; i++) {
            int bw = 70 + (i % 3) * 24;
            int bh = 90 + (i % 5) * 28;
            int bx = i * 165 - 35;
            int by = (int)(roofY - 92 - bh);
            DrawRectangle(bx, by, bw, bh, Color{ 9, 12, 24, 255 });
            DrawRectangleLines(bx, by, bw, bh, Color{ 24, 28, 45, 255 });
            for (int wy = by + 16; wy < by + bh - 10; wy += 24) {
                for (int wx = bx + 12; wx < bx + bw - 10; wx += 22) {
                    if (((wx + wy + i) % 3) == 0) DrawRectangle(wx, wy, 5, 8, Fade(GOLD, 0.45f));
                }
            }
        }

        // --- крыша: парапет, ограждение и бетонная площадка под уменьшенного игрока ---
        DrawRectangle(0, (int)(roofY - 92), scrW, 56, Color{ 35, 35, 45, 255 });
        DrawRectangle(0, (int)(roofY - 92), scrW, 4, GRAY);
        DrawRectangle(0, (int)(roofY - 36), scrW, 8, Color{ 55, 55, 65, 255 });
        for (int i = 0; i < 15; i++) {
            int postX = i * (scrW / 14);
            DrawRectangle(postX, (int)(roofY - 132), 5, 42, Color{ 80, 82, 95, 255 });
        }
        DrawRectangle(0, (int)(roofY - 132), scrW, 4, Color{ 95, 96, 110, 255 });
        DrawRectangle(0, (int)(roofY - 112), scrW, 3, Color{ 75, 76, 90, 255 });
        DrawGlow({ scrW * 0.20f, roofY - 90.0f }, 50.0f, { 255, 60, 60, 120 }, { 255, 60, 60, 0 }, 6);

        // --- бетон крыши ---
        float floorY = scrH * 0.82f;
        DrawRectangle(0, (int)(roofY - 28), scrW, (int)(floorY - roofY + 28), Color{ 42, 42, 50, 255 });
        DrawRectangle(0, (int)(floorY - 2), scrW, 5, Color{ 70, 70, 82, 255 });
        for (int x = -40; x < scrW; x += 160) DrawLineEx({ (float)x, roofY - 24.0f }, { (float)(x + 105), floorY + 28.0f }, 1.0f, Color{ 58, 58, 68, 255 });
        for (int x = 0; x < scrW; x += 95) DrawRectangle(x, (int)(floorY - 8), 48, 5, Color{ 95, 78, 35, 255 });
        DrawRectangle((int)(scrW * 0.06f), (int)(roofY - 24), 88, 32, Color{ 32, 34, 42, 255 });
        DrawRectangleLines((int)(scrW * 0.06f), (int)(roofY - 24), 88, 32, Color{ 85, 88, 105, 255 });
        DrawRectangle((int)(scrW * 0.82f), (int)(roofY - 20), 120, 28, Color{ 32, 34, 42, 255 });
        DrawRectangleLines((int)(scrW * 0.82f), (int)(roofY - 20), 120, 28, Color{ 85, 88, 105, 255 });
        if (assets.tex_floor.width > 0) {
            int tileCount = scrW / assets.tex_floor.width + 2;
            for (int i = 0; i < tileCount; i++) {
                DrawTexture(assets.tex_floor, i * assets.tex_floor.width, (int)floorY, WHITE);
            }
            DrawRectangle(0, (int)(floorY + assets.tex_floor.height), scrW, scrH - (int)(floorY + assets.tex_floor.height), Color{18, 18, 22, 255});
        }

        // --- заголовок ---
        RuText(assets.font, u8"КРЫША — 66-й ЭТАЖ", scrW / 2 - 150, 30, 28, LIGHTGRAY);
        RuText(assets.font, u8"Дальше лифт не идёт.", scrW / 2 - 100, 65, 18, GRAY);

        // --- выбор, пока не выбран ---
        if (!choiceMade) {
            RuText(assets.font, u8"Выберите свой путь:", scrW / 2 - 100, scrH * 0.56f, 20, RAYWHITE);

            // рюкзак (слева — хорошая концовка)
            {
                float cx = backpackPos.x;
                float cy = backpackPos.y;
                float cw = 160, ch = 105;

                Color bg = (hoveredChoice == 0)
                    ? Fade(GREEN, 0.25f) : Fade(DARKGREEN, 0.18f);
                DrawRectangle(cx, cy, cw, ch, bg);
                DrawRectangleLines(cx, cy, cw, ch,
                    (hoveredChoice == 0) ? YELLOW : GRAY);

                DrawGlow({ cx + cw * 0.5f, cy + ch * 0.5f }, 42.0f, { 80, 255, 120, 80 }, { 80, 255, 120, 0 }, 6);

                RuText(assets.font, u8"РЮКЗАК", cx + 34, cy + 10, 18,
                    (hoveredChoice == 0) ? YELLOW : LIGHTGRAY);
                // простой ASCII-рюкзак
                RuText(assets.font, u8"[|||||]", cx + 43, cy + 36, 14, BROWN);
                RuText(assets.font, u8"[|||||]", cx + 43, cy + 52, 14, BROWN);
                RuText(assets.font, u8"[E] ПРИНЯТЬ", cx + 36, cy + 78, 13,
                    (hoveredChoice == 0) ? YELLOW : GRAY);
            }

            // вертолёт (справа — плохая концовка)
            {
                float cx = heliPos.x;
                float cy = heliPos.y;
                float cw = 160, ch = 105;

                Color bg = (hoveredChoice == 1)
                    ? Fade(SKYBLUE, 0.35f) : Fade(DARKBLUE, 0.25f);
                DrawRectangle(cx, cy, cw, ch, bg);
                DrawRectangleLines(cx, cy, cw, ch,
                    (hoveredChoice == 1) ? YELLOW : GRAY);

                DrawGlow({ cx + cw * 0.5f, cy + ch * 0.5f }, 42.0f, { 100, 180, 255, 80 }, { 100, 180, 255, 0 }, 6);

                RuText(assets.font, u8"ВЕРТОЛЁТ", cx + 24, cy + 10, 18,
                    (hoveredChoice == 1) ? YELLOW : LIGHTGRAY);
                // простой ASCII-вертолёт
                RuText(assets.font, u8"  /\\", cx + 48, cy + 34, 14, RAYWHITE);
                RuText(assets.font, u8" /  \\", cx + 43, cy + 49, 14, RAYWHITE);
                RuText(assets.font, u8"|====|", cx + 39, cy + 64, 14, RAYWHITE);
                RuText(assets.font, u8"[E] УЛЕТЕТЬ", cx + 34, cy + 78, 13,
                    (hoveredChoice == 1) ? YELLOW : GRAY);
            }
        }

        // --- записка на бетоне ---
        DrawTextureEx(assets.tex_note, { notePosX, scrH * 0.76f }, 0.0f, 0.45f, WHITE);
        if (noteNear && !choiceMade && !noteOpen) {
            RuText(assets.font, u8"[E] прочитать", (int)(notePosX - 8), (int)(scrH * 0.84f), 12, YELLOW);
        }

        // --- плохая концовка: белый fade ---
        if (badEnding && fadeAlpha > 0.0f) {
            DrawRectangle(0, 0, scrW, scrH, Fade(RAYWHITE, fadeAlpha));

            if (fadeAlpha > 0.6f) {
                RuText(assets.font, u8"ты выбрал вертолёт...",
                         scrW / 2 - 170, scrH / 2 - 20, 28,
                         Fade(BLACK, (fadeAlpha - 0.6f) / 0.4f));
            }
        }

        // --- хорошая концовка: сообщение ---
        if (completed && !badEnding) {
            DrawRectangle(scrW / 2 - 280, scrH / 2 - 60, 560, 120,
                          Fade(BLACK, 0.85f));
            DrawRectangleLines(scrW / 2 - 280, scrH / 2 - 60, 560, 120, GOLD);
            RuText(assets.font, u8"Ты выбрал РЮКЗАК.", scrW / 2 - 130, scrH / 2 - 40, 26, GREEN);
            RuText(assets.font, u8"Ты не учёный. И никогда им не был.", scrW / 2 - 200, scrH / 2 + 5, 20, RAYWHITE);
            RuText(assets.font, u8"Но ты всё ещё человек. Пошли отсюда.", scrW / 2 - 180, scrH / 2 + 35, 20, RAYWHITE);
        }

        player.DrawSprite(assets);
    }

    void DrawUI(Assets& assets) {
        if (!noteOpen || choiceMade) return;

        int scrW = GetScreenWidth();
        int scrH = GetScreenHeight();
        int nx = scrW / 2 - 280;
        int ny = scrH / 2 - 170;
        DrawRectangle(nx, ny, 560, 340, Fade(BLACK, 0.94f));
        DrawRectangleLines(nx, ny, 560, 340, GOLD);

        RuText(assets.font, u8"--- ЗАКЛЮЧИТЕЛЬНЫЙ ОТЧЁТ ---", nx + 24, ny + 20, 22, GOLD);
        RuText(assets.font, u8"Объект №12 вышел на крышу в 03:17.", nx + 24, ny + 62, 17, RAYWHITE);
        RuText(assets.font, u8"Халат из ячейки B-7 закрепил ложную", nx + 24, ny + 91, 17, RAYWHITE);
        RuText(assets.font, u8"личность: «сотрудник комплекса».", nx + 24, ny + 119, 17, RAYWHITE);
        RuText(assets.font, u8"Доза Z-облучения: критическая.", nx + 24, ny + 156, 17, RED);
        RuText(assets.font, u8"Объект — НЕ СОТРУДНИК. Он заблудился.", nx + 24, ny + 196, 19, RAYWHITE);
        RuText(assets.font, u8"Рюкзак — реальность. Вертолёт — симптом.", nx + 24, ny + 232, 17, YELLOW);
        RuText(assets.font, u8"[E] закрыть", nx + 230, ny + 292, 13, GRAY);
    }

};
