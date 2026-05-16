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
    bool noteActive = false;
    bool choiceMade = false;
    float fadeAlpha = 0.0f;
    int hoveredChoice = -1; // -1 = ничего, 0 = рюкзак, 1 = вертолёт

    void Init(int screenW, int screenH) {
        completed = false;
        badEnding = false;
        noteActive = false;
        choiceMade = false;
        fadeAlpha = 0.0f;
        hoveredChoice = -1;

        backpackPos = { screenW * 0.18f, screenH * 0.55f };
        heliPos    = { screenW * 0.68f, screenH * 0.55f };

        notePosX = screenW * 0.5f;
    }

    void Update(Player& player) {
        // записка по центру
        noteActive = player.is_in_area(notePosX);
        hoveredChoice = -1;

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

        // --- парапет (край крыши) ---
        DrawRectangle(0, scrH * 0.50f, scrW, scrH * 0.06f, { 35, 35, 45, 255 });
        DrawRectangle(0, scrH * 0.50f, scrW, 4, GRAY);
        DrawGlow({ scrW * 0.20f, scrH * 0.50f }, 50.0f, { 255, 60, 60, 120 }, { 255, 60, 60, 0 }, 6);

        // --- здание ниже парапета ---
        DrawRectangle(0, scrH * 0.56f, scrW, scrH * 0.44f, { 40, 40, 50, 255 });

        // --- заголовок ---
        RuText(assets.font, u8"КРЫША — ЭТАЖ 5", scrW / 2 - 130, 30, 28, LIGHTGRAY);
        RuText(assets.font, u8"Лифт дальше не едет.", scrW / 2 - 110, 65, 18, GRAY);

        // --- выбор, пока не выбран ---
        if (!choiceMade) {
            RuText(assets.font, u8"Выберите свой путь:", scrW / 2 - 100, scrH * 0.42f, 20, RAYWHITE);

            // рюкзак (слева — хорошая концовка)
            {
                float cx = backpackPos.x;
                float cy = backpackPos.y;
                float cw = 200, ch = 130;

                Color bg = (hoveredChoice == 0)
                    ? Fade(GREEN, 0.25f) : Fade(DARKGREEN, 0.18f);
                DrawRectangle(cx, cy, cw, ch, bg);
                DrawRectangleLines(cx, cy, cw, ch,
                    (hoveredChoice == 0) ? YELLOW : GRAY);

                DrawGlow({ cx + cw * 0.5f, cy + ch * 0.5f }, 60.0f, { 80, 255, 120, 80 }, { 80, 255, 120, 0 }, 6);

                RuText(assets.font, u8"РЮКЗАК", cx + 45, cy + 12, 20,
                    (hoveredChoice == 0) ? YELLOW : LIGHTGRAY);
                // простой ASCII-рюкзак
                RuText(assets.font, u8"[|||||]", cx + 55, cy + 42, 16, BROWN);
                RuText(assets.font, u8"[|||||]", cx + 55, cy + 60, 16, BROWN);
                RuText(assets.font, u8"[E] ПРИНЯТЬ РЕАЛЬНОСТЬ", cx + 10, cy + 90, 14,
                    (hoveredChoice == 0) ? YELLOW : GRAY);
            }

            // вертолёт (справа — плохая концовка)
            {
                float cx = heliPos.x;
                float cy = heliPos.y;
                float cw = 200, ch = 130;

                Color bg = (hoveredChoice == 1)
                    ? Fade(SKYBLUE, 0.35f) : Fade(DARKBLUE, 0.25f);
                DrawRectangle(cx, cy, cw, ch, bg);
                DrawRectangleLines(cx, cy, cw, ch,
                    (hoveredChoice == 1) ? YELLOW : GRAY);

                DrawGlow({ cx + cw * 0.5f, cy + ch * 0.5f }, 60.0f, { 100, 180, 255, 80 }, { 100, 180, 255, 0 }, 6);

                RuText(assets.font, u8"ВЕРТОЛЁТ", cx + 35, cy + 12, 20,
                    (hoveredChoice == 1) ? YELLOW : LIGHTGRAY);
                // простой ASCII-вертолёт
                RuText(assets.font, u8"  /\\", cx + 60, cy + 38, 16, RAYWHITE);
                RuText(assets.font, u8" /  \\", cx + 55, cy + 54, 16, RAYWHITE);
                RuText(assets.font, u8"|====|", cx + 50, cy + 70, 16, RAYWHITE);
                RuText(assets.font, u8"[E] УЛЕТЕТЬ", cx + 45, cy + 90, 14,
                    (hoveredChoice == 1) ? YELLOW : GRAY);
            }
        }

        // --- записка на полу ---
        DrawRectangle(notePosX, scrH * 0.70f, 36, 28, BROWN);
        DrawRectangleLines(notePosX, scrH * 0.70f, 36, 28, DARKBROWN);
        RuText(assets.font, u8"последняя записка", notePosX - 14, scrH * 0.67f, 13, BEIGE);

        // --- текст записки ---
        if (noteActive && !choiceMade) {
            int nx = scrW / 2 - 420;
            int ny = scrH / 2 - 280;
            DrawRectangle(nx, ny, 840, 560, Fade(BLACK, 0.94f));
            DrawRectangleLines(nx, ny, 840, 560, GOLD);

            RuText(assets.font, u8"--- ЗАКЛЮЧИТЕЛЬНЫЙ ОТЧЁТ ---", nx + 30, ny + 25, 26, GOLD);
            RuText(assets.font, u8"Объект №12 вырвался из изоляции.", nx + 30, ny + 70, 22, RAYWHITE);
            RuText(assets.font, u8"В последний раз видели в лабораторном халате,", nx + 50, ny + 105, 22, RAYWHITE);
            RuText(assets.font, u8"взятом из шкафчика B-7.", nx + 50, ny + 135, 22, RAYWHITE);
            RuText(assets.font, u8"Доза Z-радиации: 4,7 Гр — КРИТИЧЕСКАЯ.", nx + 30, ny + 175, 22, RED);
            RuText(assets.font, u8"Прогноз: полный крах личности", nx + 50, ny + 210, 22, RED);
            RuText(assets.font, u8"в течение 72 часов. Объект утратит", nx + 50, ny + 240, 22, RED);
            RuText(assets.font, u8"всё чувство себя, приняв личность", nx + 50, ny + 270, 22, RED);
            RuText(assets.font, u8"того, чью одежду он надел.", nx + 50, ny + 300, 22, RED);
            RuText(assets.font, u8"Объект — НЕ учёный.", nx + 30, ny + 350, 26, RAYWHITE);
            RuText(assets.font, u8"Он — НЕЗВАНЫЙ ГОСТЬ, который заблудился.", nx + 50, ny + 385, 26, RAYWHITE);
            RuText(assets.font, u8"При обнаружении НЕ ПРИБЛИЖАТЬСЯ. Он", nx + 30, ny + 435, 20, RAYWHITE);
            RuText(assets.font, u8"вооружён ГАЛЛЮЦИНАЦИЯМИ и лабораторным халатом.", nx + 50, ny + 465, 20, RAYWHITE);
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
            RuText(assets.font, u8"Ты не учёный. Ты им никогда не был.", scrW / 2 - 200, scrH / 2 + 5, 20, RAYWHITE);
            RuText(assets.font, u8"Но, возможно... это нормально.", scrW / 2 - 150, scrH / 2 + 35, 20, RAYWHITE);
        }

        player.Draw();
    }
};
