#pragma once
#include <raylib.h>
#include "assets.h"

enum GameState { MENU, PLAYING, EXIT };

struct Menu {
    GameState state = MENU;
    bool continueSelected = false;
    int hoveredBtn = -1;

    void Init() {
        state = MENU;
        continueSelected = false;
        hoveredBtn = -1;
    }

    void Update(Font font) {
        (void)font;
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        float bw = 300.0f;
        float bh = 50.0f;
        float baseY = sh * 0.55f;
        float gap = 65.0f;

        Vector2 mouse = GetMousePosition();
        hoveredBtn = -1;

        Rectangle btnPlay = { (sw - bw) / 2.0f, baseY, bw, bh };
        Rectangle btnQuit = { (sw - bw) / 2.0f, baseY + gap, bw, bh };

        if (CheckCollisionPointRec(mouse, btnPlay)) hoveredBtn = 0;
        if (CheckCollisionPointRec(mouse, btnQuit)) hoveredBtn = 1;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (hoveredBtn == 0) {
                continueSelected = false;
                state = PLAYING;
            } else if (hoveredBtn == 1) {
                state = EXIT;
            }
        }
    }

    void Draw(Font font) {
        ClearBackground({ 8, 8, 14, 255 });

        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        float bw = 300.0f;
        float bh = 50.0f;
        float baseY = sh * 0.55f;
        float gap = 65.0f;

        int titleW = RuMeasure(font, u8"ЛИФТ", 72);
        RuText(font, u8"ЛИФТ", (sw - titleW) / 2, 120, 72, RAYWHITE);

        int subW = RuMeasure(font, u8"Психологический хоррор в лифте", 24);
        RuText(font, u8"Психологический хоррор в лифте", (sw - subW) / 2, 200, 24, GRAY);

        const char* labels[] = { u8"ИГРАТЬ", u8"ВЫХОД" };
        float ys[] = { baseY, baseY + gap };

        for (int i = 0; i < 2; i++) {
            Rectangle rec = { (sw - bw) / 2.0f, ys[i], bw, bh };
            Color bg = (hoveredBtn == i) ? Color{ 50, 50, 60, 255 } : Color{ 30, 30, 38, 255 };
            Color border = (hoveredBtn == i) ? RAYWHITE : Color{ 100, 100, 110, 255 };
            DrawRectangleRec(rec, bg);
            DrawRectangleLinesEx(rec, 2, border);
            int tw = RuMeasure(font, labels[i], 28);
            RuText(font, labels[i], (int)(rec.x + (bw - tw) / 2.0f), (int)(rec.y + 12), 28, RAYWHITE);
        }

        int hintW = RuMeasure(font, u8"Управление: мышь / клавиатура", 18);
        RuText(font, u8"Управление: мышь / клавиатура", (sw - hintW) / 2, sh - 50, 18, Color{ 80, 80, 90, 255 });
    }
};
