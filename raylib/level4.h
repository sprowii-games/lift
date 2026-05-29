#pragma once
#include "level.h"
#include "../utils.h"
#include <cmath>

struct Level4 {
    bool completed = false;
    bool badEnding = false;

    Vector2 doorPos;
    float notePosX;
    bool noteActive;

    float floorY;
    float tableH;

    Vector2 boxPos;
    bool catAlive;

    Vector2 cameraPos;

    Vector2 gunPos;
    Vector2 screenPos;

    Vector2 btnWavePos;
    Vector2 btnParticlePos;

    enum State {
        WAITING,
        BOX_OPEN,
        EXPERIMENTS_UNLOCKED,
        SHOW_WAVES,
        SHOW_PARTICLES,
        QUIZ,
        SOLVED
    };
    State state;
    float stateTimer;

    bool waveSeen;
    bool particleSeen;

    int quizAnswer;
    int quizHovered;

    void Init(int screenW, int screenH) {
        completed = false;
        badEnding = false;
        noteActive = false;
        state = WAITING;
        stateTimer = 0.0f;
        waveSeen = false;
        particleSeen = false;
        catAlive = false;
        quizAnswer = -1;
        quizHovered = -1;

        floorY = screenH * 0.82f;
        tableH = 65.0f;

        doorPos = { screenW * 0.88f, floorY };
        notePosX = screenW * 0.06f;

        boxPos          = { screenW * 0.22f, floorY - tableH - 25 };
        cameraPos       = { screenW * 0.44f, floorY - tableH - 25 };
        gunPos          = { screenW * 0.65f, floorY - tableH - 15 };
        screenPos       = { screenW * 0.65f, floorY - 155.0f };
        btnWavePos      = { screenW * 0.57f, floorY - 35.0f };
        btnParticlePos  = { screenW * 0.73f, floorY - 35.0f };
    }

    void Update(Player& player) {
        float dt = GetFrameTime();
        noteActive = player.is_in_area(notePosX);

        if (state == WAITING) {
            if (player.is_in_area(boxPos.x) && IsKeyPressed(KEY_E)) {
                catAlive = (GetRandomValue(0, 1) == 1);
                state = BOX_OPEN;
                stateTimer = 2.0f;
            }
            return;
        }

        if (state == BOX_OPEN) {
            stateTimer -= dt;
            if (stateTimer <= 0.0f) {
                state = EXPERIMENTS_UNLOCKED;
            }
            return;
        }

        if (state == EXPERIMENTS_UNLOCKED) {
            if (player.is_in_area(btnWavePos.x) && IsKeyPressed(KEY_E)) {
                state = SHOW_WAVES;
                stateTimer = 2.0f;
            }
            if (player.is_in_area(btnParticlePos.x) && IsKeyPressed(KEY_E)) {
                state = SHOW_PARTICLES;
                stateTimer = 2.0f;
            }
            return;
        }

        if (state == SHOW_WAVES) {
            stateTimer -= dt;
            if (stateTimer <= 0.0f) {
                waveSeen = true;
                state = (waveSeen && particleSeen) ? QUIZ : EXPERIMENTS_UNLOCKED;
            }
            return;
        }

        if (state == SHOW_PARTICLES) {
            stateTimer -= dt;
            if (stateTimer <= 0.0f) {
                particleSeen = true;
                state = (waveSeen && particleSeen) ? QUIZ : EXPERIMENTS_UNLOCKED;
            }
            return;
        }

        if (state == QUIZ) {
            quizHovered = -1;
            float quizBaseX = GetScreenWidth() * 0.35f;
            for (int i = 0; i < 3; i++) {
                float bx = quizBaseX + i * 150.0f;
                if (player.is_in_area(bx)) {
                    quizHovered = i;
                    if (IsKeyPressed(KEY_E)) {
                        quizAnswer = i;
                        if (i == 2) {
                            state = SOLVED;
                        }
                    }
                }
            }
            return;
        }

        if (state == SOLVED) {
            if (player.is_in_area(doorPos.x) && IsKeyPressed(KEY_E)) {
                completed = true;
            }
        }
    }

    void DrawWavePattern(Assets& assets, float cx, float cy, float w, float h) {
        DrawRectangle((int)(cx - w / 2), (int)(cy - h / 2), (int)w, (int)h, Color{ 5, 10, 5, 255 });

        float baseY = cy;
        for (int i = -6; i <= 6; i++) {
            float x = cx + i * (w / 14.0f);
            float intensity = cosf(i * 0.35f);
            if (intensity < 0.0f) intensity = 0.0f;
            float barH = h * 0.8f * intensity;
            Color c = Color{
                (unsigned char)(30 * intensity),
                (unsigned char)(255 * intensity),
                (unsigned char)(100 * intensity),
                255
            };
            DrawLineEx({ x, baseY - barH / 2 }, { x, baseY + barH / 2 }, 3.0f, c);
        }

        RuText(assets.font, u8"\u0418\u043d\u0442\u0435\u0440\u0444\u0435\u0440\u0435\u043d\u0446\u0438\u044f",
               (int)(cx - 55), (int)(cy + h / 2 + 4), 12, GREEN);
    }

    void DrawParticlePattern(Assets& assets, float cx, float cy, float w, float h) {
        DrawRectangle((int)(cx - w / 2), (int)(cy - h / 2), (int)w, (int)h, Color{ 5, 5, 10, 255 });

        float gap = w * 0.25f;
        for (int s = 0; s < 2; s++) {
            float sx = cx - gap / 2 + s * gap;
            Color c = Color{ 60, 120, 255, 255 };
            DrawLineEx({ sx, cy - h * 0.35f }, { sx, cy + h * 0.35f }, 6.0f, c);
            for (int d = 0; d < 5; d++) {
                float dx = RandF(-8, 8);
                float dy = RandF(-h * 0.3f, h * 0.3f);
                DrawCircleV({ sx + dx, cy + dy }, 2, Fade(c, 0.4f));
            }
        }

        RuText(assets.font, u8"\u0414\u0432\u0435 \u0449\u0435\u043b\u0438",
               (int)(cx - 35), (int)(cy + h / 2 + 4), 12, SKYBLUE);
    }

    void DrawLabTable(float cx, float surfY, float w, float surfH) {
        DrawRectangle((int)(cx - w / 2), (int)surfY, (int)w, (int)surfH, Color{ 45, 38, 30, 255 });
        DrawRectangleLines((int)(cx - w / 2), (int)surfY, (int)w, (int)surfH, Color{ 85, 70, 50, 255 });

        float legW = 6.0f;
        float legTop = surfY + surfH;
        float legH = floorY - legTop;
        Color legCol = { 55, 48, 38, 255 };
        if (legH > 0) {
            DrawRectangle((int)(cx - w / 2 + 5), (int)legTop, (int)legW, (int)legH, legCol);
            DrawRectangle((int)(cx + w / 2 - 5 - legW), (int)legTop, (int)legW, (int)legH, legCol);
        }
    }

    void Draw(Player& player, Assets& assets) {
        int scrW = GetScreenWidth();
        int scrH = GetScreenHeight();

        ClearBackground(Color{ 10, 15, 10, 255 });

        float surfaceY = floorY - tableH;

        if (assets.tex_wall.width > 0) {
            int wallTileCount = scrW / assets.tex_wall.width + 2;
            for (int i = 0; i < wallTileCount; i++) {
                DrawTexture(assets.tex_wall, i * assets.tex_wall.width, (int)(floorY - assets.tex_wall.height), WHITE);
            }
        }

        if (assets.tex_floor.width > 0) {
            int tileCount = scrW / assets.tex_floor.width + 2;
            for (int i = 0; i < tileCount; i++) {
                DrawTexture(assets.tex_floor, i * assets.tex_floor.width, (int)floorY, WHITE);
            }
            DrawRectangle(0, (int)(floorY + assets.tex_floor.height), scrW, scrH - (int)(floorY + assets.tex_floor.height), Color{18, 18, 22, 255});
        }

        RuText(assets.font, u8"\u042d\u0422\u0410\u0416 3 : \u041b\u0410\u0411\u041e\u0420\u0410\u0422\u041e\u0420\u0418\u042f",
               300, (int)(floorY - 195), 24, LIGHTGRAY);
        RuText(assets.font, u8"Z-\u0438\u0437\u043b\u0443\u0447\u0435\u043d\u0438\u0435 \u00b7 \u043a\u0432\u0430\u043d\u0442\u043e\u0432\u044b\u0435 \u044d\u043a\u0441\u043f\u0435\u0440\u0438\u043c\u0435\u043d\u0442\u044b",
               300, (int)(floorY - 168), 14, GRAY);

        // --- Записка на столе ---
        DrawLabTable(notePosX + 25, surfaceY, 60, 10);
        DrawTexture(assets.tex_note, (int)notePosX, (int)(surfaceY - 18), WHITE);

        // --- Коробка Шрёдингера на столе ---
        DrawLabTable(boxPos.x, surfaceY, 100, 12);

        float bxW = 55, bxH = 45;
        float bxX = boxPos.x - bxW / 2;
        float bxY = surfaceY - bxH;
        DrawRectangle((int)bxX, (int)bxY, (int)bxW, (int)bxH, Color{ 50, 40, 60, 255 });
        DrawRectangleLines((int)bxX, (int)bxY, (int)bxW, (int)bxH, PURPLE);
        RuText(assets.font, "?", (int)(boxPos.x - 7), (int)(bxY + 10), 22, PURPLE);

        if (state == WAITING || state == BOX_OPEN) {
            RuText(assets.font, u8"\u041a\u041e\u0420\u041e\u0411\u041a\u0410 \u0428\u0420\u0401\u0414\u0418\u041d\u0413\u0415\u0420\u0410",
                   (int)(boxPos.x - 78), (int)(bxY - 18), 12, PURPLE);
        }

        if (state == WAITING && player.is_in_area(boxPos.x)) {
            RuText(assets.font, u8"[E] \u041e\u0442\u043a\u0440\u044b\u0442\u044c",
                   (int)(boxPos.x - 35), (int)(floorY + 8), 13, YELLOW);
        }

        // Результат коробки
        if (state == BOX_OPEN) {
            Color catCol = catAlive ? GREEN : RED;
            const char* catText = catAlive
                ? u8"\u0421\u0443\u043f\u0435\u0440\u043f\u043e\u0437\u0438\u0446\u0438\u044f \u0440\u0443\u0445\u043d\u0443\u043b\u0430. \u041a\u043e\u0442: \u0416\u0418\u0412"
                : u8"\u0421\u0443\u043f\u0435\u0440\u043f\u043e\u0437\u0438\u0446\u0438\u044f \u0440\u0443\u0445\u043d\u0443\u043b\u0430. \u041a\u043e\u0442: \u041c\u0401\u0420\u0422\u0412";
            int tw = RuMeasure(assets.font, catText, 16);
            float ovX = player.pos.x - tw / 2 - 8;
            float ovY = surfaceY - 55;
            DrawRectangle((int)ovX, (int)ovY, tw + 16, 38, Fade(BLACK, 0.85f));
            RuText(assets.font, catText, (int)(ovX + 8), (int)(ovY + 8), 16, catCol);
        }

        // --- Z-камера на столе ---
        DrawLabTable(cameraPos.x, surfaceY, 120, 14);

        float camDrawY = surfaceY - 45;
        DrawGlow({ cameraPos.x, camDrawY + 18 }, 35.0f, { 100, 255, 100, 80 }, { 50, 200, 50, 0 }, 6);
        DrawRectangle((int)(cameraPos.x - 30), (int)camDrawY, 60, 40, Color{ 30, 50, 30, 255 });
        DrawRectangleLines((int)(cameraPos.x - 30), (int)camDrawY, 60, 40, GREEN);
        RuText(assets.font, "Z", (int)(cameraPos.x - 5), (int)(camDrawY + 8), 20, GREEN);
        RuText(assets.font, u8"Z-\u043a\u0430\u043c\u0435\u0440\u0430",
               (int)(cameraPos.x - 35), (int)(camDrawY - 15), 12, Color{ 100, 255, 100, 180 });

        // Кабель: камера → пушка
        DrawLineEx({ cameraPos.x + 30, surfaceY - 3 }, { gunPos.x - 30, surfaceY - 3 }, 2.0f, Color{ 60, 100, 60, 120 });

        // --- Экспериментальный стол (пушка + кнопки) ---
        DrawLabTable(gunPos.x, surfaceY, 180, 14);

        // Пушка на столе
        float gunDrawY = surfaceY - 30;
        DrawRectangle((int)(gunPos.x - 20), (int)gunDrawY, 40, 25, Color{ 30, 30, 50, 255 });
        DrawRectangleLines((int)(gunPos.x - 20), (int)gunDrawY, 40, 25, SKYBLUE);
        DrawRectangle((int)(gunPos.x - 3), (int)(gunDrawY - 15), 6, 15, Color{ 40, 40, 70, 255 });
        RuText(assets.font, u8"e\u207b \u043f\u0443\u0448\u043a\u0430",
               (int)(gunPos.x - 25), (int)(gunDrawY - 28), 12, SKYBLUE);

        // Экран на стене
        float scW = 110, scH = 70;
        float scX = screenPos.x - scW / 2;
        float scY = screenPos.y - scH / 2;
        DrawRectangle((int)(scX - 5), (int)(scY - 5), (int)(scW + 10), (int)(scH + 10), Color{ 25, 30, 25, 255 });
        DrawRectangleLines((int)(scX - 5), (int)(scY - 5), (int)(scW + 10), (int)(scH + 10), Color{ 60, 100, 60, 255 });
        DrawRectangle((int)scX, (int)scY, (int)scW, (int)scH, Color{ 15, 20, 15, 255 });
        // Кронштейн
        DrawRectangle((int)(screenPos.x - 2), (int)(scY + scH), 4, (int)(surfaceY - scY - scH + 14), Color{ 50, 55, 50, 255 });

        RuText(assets.font, u8"\u042d\u043a\u0440\u0430\u043d",
               (int)(screenPos.x - 20), (int)(scY - 15), 12, Color{ 60, 100, 60, 180 });

        if (state == SHOW_WAVES) {
            DrawWavePattern(assets, screenPos.x, screenPos.y, 95.0f, 55.0f);
        } else if (state == SHOW_PARTICLES) {
            DrawParticlePattern(assets, screenPos.x, screenPos.y, 95.0f, 55.0f);
        } else if (state == EXPERIMENTS_UNLOCKED || state == QUIZ) {
            RuText(assets.font, "...", (int)(screenPos.x - 8), (int)(screenPos.y - 6), 14, DARKGREEN);
        }

        // Кнопки экспериментов
        if (state == EXPERIMENTS_UNLOCKED) {
            Color btn1Col = player.is_in_area(btnWavePos.x)
                ? Color{ 40, 80, 40, 255 } : Color{ 25, 40, 25, 255 };
            DrawRectangle((int)(btnWavePos.x - 58), (int)(btnWavePos.y - 11), 116, 22, btn1Col);
            DrawRectangleLines((int)(btnWavePos.x - 58), (int)(btnWavePos.y - 11), 116, 22, GREEN);
            RuText(assets.font, u8"\u0411\u0415\u0417 \u0414\u0415\u0422\u0415\u041a\u0422\u041e\u0420\u0410",
                   (int)(btnWavePos.x - 52), (int)(btnWavePos.y - 8), 11, GREEN);
            if (player.is_in_area(btnWavePos.x)) {
                RuText(assets.font, u8"[E]", (int)(btnWavePos.x - 8), (int)(btnWavePos.y - 24), 12, YELLOW);
            }

            Color btn2Col = player.is_in_area(btnParticlePos.x)
                ? Color{ 40, 40, 80, 255 } : Color{ 25, 25, 40, 255 };
            DrawRectangle((int)(btnParticlePos.x - 53), (int)(btnParticlePos.y - 11), 106, 22, btn2Col);
            DrawRectangleLines((int)(btnParticlePos.x - 53), (int)(btnParticlePos.y - 11), 106, 22, SKYBLUE);
            RuText(assets.font, u8"\u0421 \u0414\u0415\u0422\u0415\u041a\u0422\u041e\u0420\u041e\u041c",
                   (int)(btnParticlePos.x - 47), (int)(btnParticlePos.y - 8), 11, SKYBLUE);
            if (player.is_in_area(btnParticlePos.x)) {
                RuText(assets.font, u8"[E]", (int)(btnParticlePos.x - 8), (int)(btnParticlePos.y - 24), 12, YELLOW);
            }

            if (waveSeen) {
                RuText(assets.font, u8"\u2713 \u0412\u043e\u043b\u043d\u044b",
                       (int)(btnWavePos.x - 30), (int)(btnWavePos.y + 14), 11, GREEN);
            }
            if (particleSeen) {
                RuText(assets.font, u8"\u2713 \u0427\u0430\u0441\u0442\u0438\u0446\u044b",
                       (int)(btnParticlePos.x - 35), (int)(btnParticlePos.y + 14), 11, SKYBLUE);
            }
        }

        // Викторина
        if (state == QUIZ) {
            float qx = scrW * 0.35f;
            float qy = floorY - 105.0f;

            const char* qText = u8"\u0427\u0442\u043e \u0432\u044b \u043d\u0430\u0431\u043b\u044e\u0434\u0430\u043b\u0438? \u042d\u043b\u0435\u043a\u0442\u0440\u043e\u043d \u2014 \u044d\u0442\u043e...";
            int tw = RuMeasure(assets.font, qText, 15);
            float qOX = player.pos.x - tw / 2 - 8;
            DrawRectangle((int)qOX, (int)(qy - 42), tw + 16, 32, Fade(BLACK, 0.9f));
            RuText(assets.font, qText, (int)(qOX + 8), (int)(qy - 36), 15, RAYWHITE);

            const char* options[3] = {
                u8"\u0427\u0430\u0441\u0442\u0438\u0446\u0430",
                u8"\u0412\u043e\u043b\u043d\u0430",
                u8"\u0418 \u0442\u043e \u0438 \u0434\u0440\u0443\u0433\u043e\u0435"
            };
            Color optColors[3] = { RED, GREEN, GOLD };

            for (int i = 0; i < 3; i++) {
                float bx = qx + i * 150.0f;
                bool hovered = (quizHovered == i);
                Color bg = hovered ? Fade(optColors[i], 0.3f) : Fade(BLACK, 0.7f);
                DrawRectangle((int)bx, (int)qy, 140, 34, bg);
                DrawRectangleLines((int)bx, (int)qy, 140, 34, optColors[i]);
                RuText(assets.font, options[i], (int)(bx + 8), (int)(qy + 8), 14, optColors[i]);
                if (hovered) {
                    RuText(assets.font, u8"[E]", (int)(bx + 55), (int)(qy - 16), 12, YELLOW);
                }
            }

            if (quizAnswer != -1 && quizAnswer != 2) {
                RuText(assets.font, u8"\u0422\u044b \u0436\u0435 \u0441\u0430\u043c \u0432\u0441\u0451 \u0432\u0438\u0434\u0435\u043b. \u041d\u0435 \u0442\u0443\u043f\u0438.",
                       (int)(player.pos.x - 90), (int)(qy + 46), 14, RED);
                quizAnswer = -1;
            }
        }

        // Записка
        if (noteActive) {
            float nx = player.pos.x - 150;
            float ny = floorY - 215;
            DrawRectangle((int)nx, (int)ny, 320, 200, Fade(BLACK, 0.92f));
            DrawRectangleLines((int)nx, (int)ny, 320, 200, GOLD);

            RuText(assets.font, u8"--- \u0417\u0410\u041f\u0418\u0421\u041a\u0410 #7 ---", (int)(nx + 12), (int)(ny + 12), 20, GOLD);

            float glitch = sinf(GetTime() * 7.0f);
            Color textColor = (glitch > 0.9f)
                ? Color{ 0, 255, 0, 255 } : RAYWHITE;

            RuText(assets.font,
                   u8"\u041e\u0431\u044a\u0435\u043a\u0442 \u0414.\u041a. \u041d\u0430\u0434\u0435\u043b \u0445\u0430\u043b\u0430\u0442 \u0438\u0437 \u0448\u043a\u0430\u0444\u0447\u0438\u043a\u0430 B-7. \u0417\u0430\u0442\u0435\u043c \u2014 \u043e\u0431\u043b\u0443\u0447\u0435\u043d\u0438\u0435.",
                   (int)(nx + 12), (int)(ny + 45), 14, textColor);
            RuText(assets.font,
                   u8"\u041a\u043e\u043d\u0444\u0430\u0431\u0443\u043b\u044f\u0446\u0438\u044f \u043f\u043e\u043b\u043d\u0430\u044f. \u041b\u0438\u0447\u043d\u043e\u0441\u0442\u044c \u0432\u044b\u0442\u0435\u0441\u043d\u0435\u043d\u0430.",
                   (int)(nx + 12), (int)(ny + 68), 14, textColor);
            RuText(assets.font,
                   u8"\u0421\u0447\u0438\u0442\u0430\u0435\u0442 \u0441\u0435\u0431\u044f \u0441\u043e\u0442\u0440\u0443\u0434\u043d\u0438\u043a\u043e\u043c. \u041d\u0435 \u0441\u043f\u043e\u0440\u0438\u0442\u044c.",
                   (int)(nx + 12), (int)(ny + 91), 14, textColor);

            if (glitch > 0.8f) {
                RuText(assets.font,
                       u8"\u041e\u041d \u041d\u0415 \u0421\u041e\u0422\u0420\u0423\u0414\u041d\u0418\u041a \u041e\u041d \u041d\u0415 \u0421\u041e\u0422\u0420\u0423\u0414\u041d\u0418\u041a",
                       (int)(nx + 12), (int)(ny + 125), 16, Color{ 255, 0, 80, 200 });
            }
        }

        // Лифт
        float doorScale = 0.2f;
        int doorH = (int)(assets.tex_elevator_closed.height * doorScale);
        if (state == SOLVED) {
            DrawTextureEx(assets.tex_elevator_opened, {doorPos.x, doorPos.y - doorH}, 0.0f, doorScale, WHITE);
            RuText(assets.font, u8"[E] \u0412\u041e\u0419\u0422\u0418 \u0412 \u041b\u0418\u0424\u0422",
                   (int)doorPos.x - 30, (int)(doorPos.y - doorH - 16), 13, GREEN);
        } else if (state == EXPERIMENTS_UNLOCKED || state == SHOW_WAVES || state == SHOW_PARTICLES || state == QUIZ) {
            DrawTextureEx(assets.tex_elevator_closed, {doorPos.x, doorPos.y - doorH}, 0.0f, doorScale, WHITE);
        }

        player.DrawSprite(assets);
    }
};
